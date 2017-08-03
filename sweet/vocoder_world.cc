// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Copyright 2017 Google, Inc.
// Author: mjansche@google.com (Martin Jansche)
//
// \file
// Analysis and synthesis with the World vocoder.

#include <cmath>
#include <iostream>
#include <vector>

#include "external/sptk/sptk.h"
#include "external/world/cheaptrick.h"
#include "external/world/dio.h"
#include "external/world/stonemask.h"
#include "external/world/tools/audioio.h"
#include "sweet/voice_data.pb.h"

namespace {

class Analysis {
 public:
  Analysis() = delete;

  Analysis(const char *path, double frame_shift_ms)
      : frame_shift_ms_(frame_shift_ms),
        num_samples_(GetAudioLength(path)) {
    if (num_samples_ <= 0) return;
    samples_.resize(num_samples_);
    temporal_positions_.resize(num_samples_);
    wavread(path, &sample_rate_, &bit_depth_, samples_.data());
    num_frames_ = GetSamplesForDIO(sample_rate_, num_samples_, frame_shift_ms_);
  }

  double FrameShiftInSeconds() const { return frame_shift_ms_ * 1e-3; }

  int num_samples() const { return num_samples_; }

  int sample_rate() const { return sample_rate_; }

  int num_frames() const { return num_frames_; }

  std::ostream &PrintSummary(std::ostream &strm) const {
    strm << "Audio length: " << num_samples_ << " samples" << std::endl;
    strm << "Sample rate: " << sample_rate_ << " Hz" << std::endl;
    strm << "Bit depth: " << bit_depth_ << " bits" << std::endl;
    return strm;
  }

  void F0() {
    DioOption dio_option;
    InitializeDioOption(&dio_option);
    dio_option.frame_period = frame_shift_ms_;
    std::vector<double> temporal_positions(num_samples_);
    std::vector<double> dio_f0(num_frames_);
    Dio(samples_.data(), samples_.size(), sample_rate_, &dio_option,
        temporal_positions_.data(), dio_f0.data());
    f0_.resize(num_frames_);
    StoneMask(samples_.data(), samples_.size(), sample_rate_,
              temporal_positions_.data(), dio_f0.data(), dio_f0.size(),
              f0_.data());
  }

  void Spectrogram() {
    CheapTrickOption cheap_trick_option;
    InitializeCheapTrickOption(sample_rate_, &cheap_trick_option);
    cheap_trick_option.q1 = -0.15;  // TODO: Double-check.
    fft_size_ = GetFFTSizeForCheapTrick(sample_rate_, &cheap_trick_option);
    const int fft_dim = fft_size_ / 2 + 1;
    spectrogram_storage_.resize(num_frames_, std::vector<double>(fft_dim));
    spectrogram_.resize(num_frames_);
    for (size_t f = 0; f < num_frames_; ++f) {
      spectrogram_[f] = spectrogram_storage_[f].data();
    }
    CheapTrick(samples_.data(), samples_.size(), sample_rate_,
               temporal_positions_.data(), f0_.data(), f0_.size(),
               &cheap_trick_option, spectrogram_.data());
  }

  void SetFrames(sweet::WorldData *world_data) {
    auto *frames = world_data->mutable_frame();
    frames->Reserve(num_frames_);
    for (size_t f = 0; f < num_frames_; ++f) {
      auto *frame = frames->Add();
      // Set log-F0:
      double x = f0_.at(f);
      float y = -1e10f;
      if (x > 0) {
        y = std::log(x);
      }
      frame->set_lf0(y);
      // Set spectrogram for debugging:
      const auto &spectrum = spectrogram_storage_.at(f);
      const int fft_dim = fft_size_ / 2 + 1;
      auto *sp = frame->mutable_sp();
      sp->Resize(fft_dim, 0.0);
      for (int s = 0; s < fft_dim; ++s) {
        sp->Set(s, spectrum.at(s));
      }
    }
  }

 private:
  const double frame_shift_ms_;
  const int num_samples_;
  int sample_rate_;
  int bit_depth_;
  std::vector<double> samples_;
  std::vector<double> temporal_positions_;
  size_t num_frames_;
  std::vector<double> f0_;
  int fft_size_;
  std::vector<std::vector<double>> spectrogram_storage_;
  std::vector<double *> spectrogram_;
};

}  // namespace

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Input file not provided" << std::endl;
    return 1;
  }
  const char *const path = argv[1];

  Analysis analysis(path, 5.0);
  analysis.PrintSummary(std::cerr);
  analysis.F0();
  analysis.Spectrogram();

  sweet::WorldData world_data;
  world_data.set_frame_shift_s(analysis.FrameShiftInSeconds());
  world_data.set_num_samples(analysis.num_samples());
  world_data.set_sample_rate_hz(analysis.sample_rate());
  analysis.SetFrames(&world_data);

  // Generate debug output.
  const double duration_s = world_data.num_samples() /
      static_cast<double>(world_data.sample_rate_hz());
  for (int i = 0; i < world_data.frame_size(); ++i) {
    float start = i * world_data.frame_shift_s();
    float end = (i + 1) * world_data.frame_shift_s();
    if (end > duration_s) end = duration_s;
    auto *frame = world_data.mutable_frame(i);
    frame->set_start(start);
    frame->set_end(end);
  }
  std::cout << world_data.Utf8DebugString();

  return 0;
}
