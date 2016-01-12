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
// Copyright 2016 Google, Inc.
// Author: mjansche@google.com (Martin Jansche)
//
// Tool for setting the sample rate of a wave file to 48kHz without resampling.
// This is needed when the sample rate indicated in the header is wrong.
//
// Compile with: gcc -std=c99 -o fix_wave_sample_rate fix_wave_sample_rate.c

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define static_assert _Static_assert

struct WaveHeader {
  char chunk_id[4];
  uint32_t chunk_size;
  char format[4];
  char subchunk1_id[4];
  uint32_t subchunk1_size;
  uint16_t audio_format;
  uint16_t num_channels;
  uint32_t sample_rate;
  uint32_t byte_rate;
  uint16_t block_align;
  uint16_t bits_per_sample;
  char subchunk2_id[4];
  uint32_t subchunk2_size;
};

bool CheckHeader(const struct WaveHeader *hdr, const char *path) {
  if (strncmp(hdr->chunk_id, "RIFF", 4) != 0 ||
      strncmp(hdr->format, "WAVE", 4) != 0 ||
      strncmp(hdr->subchunk1_id, "fmt ", 4) != 0) {
    fprintf(stderr, "%s: No valid WAVE header detected\n", path);
    return false;
  }
  // This program only knows how to handle audio_format 1 (linear PCM):
  if (hdr->subchunk1_size != 16 && hdr->audio_format != 1) {
    fprintf(stderr, "%s: Audio format 0x%X unknown, fmt chunk size is %u\n",
            path, hdr->audio_format, hdr->subchunk1_size);
    return false;
  }
  if (hdr->block_align != hdr->num_channels * hdr->bits_per_sample / 8) {
    fprintf(stderr, "%s: Byte alignment and sample depth are inconsistent: "
            "%u bits per sample, %u channel(s), alignment is %u\n", path,
            hdr->bits_per_sample, hdr->num_channels, hdr->block_align);
    return false;
  }
  return true;
}

void PrettyPrintHeader(const struct WaveHeader *hdr, const char *path) {
  fprintf(stderr, "%s: %u-bit PCM, ", path, hdr->bits_per_sample);
  if (hdr->num_channels == 1) {
    fprintf(stderr, "mono");
  } else if (hdr->num_channels == 2) {
    fprintf(stderr, "stereo");
  } else {
    fprintf(stderr, "%u channels", hdr->num_channels);
  }
  fprintf(stderr, ", sample rate %u Hz, byte rate %u Bps\n",
          hdr->sample_rate, hdr->byte_rate);
}

bool CheckSampleRate(const struct WaveHeader *hdr, const char *path) {
  float expected_sample_rate =
      hdr->byte_rate * 8.0f / (hdr->bits_per_sample * hdr->num_channels);
  if (hdr->sample_rate != expected_sample_rate) {
    fprintf(stderr, "%s: Expected sample rate %.2f Hz, but found %u Hz\n",
            path, expected_sample_rate, hdr->sample_rate);
    return false;
  }
  return true;
}

void FixSampleRate(const char *path) {
  FILE *f = fopen(path, "r+");
  if (!f) {
    perror(path);
    return;
  }
  struct WaveHeader header;
  static const int kWaveHeaderSize = sizeof(struct WaveHeader);
  static_assert(sizeof(struct WaveHeader) == 44, "unexpected header size");
  if (fread(&header, kWaveHeaderSize, 1, f) != 1) {
    if (ferror(f)) {
      perror(path);
    } else {
      fprintf(stderr, "%s: Could not read wave header (file too short?)\n",
              path);
    }
    fclose(f);
    return;
  }
  if (!CheckHeader(&header, path)) {
    fclose(f);
    return;
  }
  PrettyPrintHeader(&header, path);
  header.sample_rate = 48000;
  if (!CheckSampleRate(&header, path)) {
    fclose(f);
    return;
  }
  rewind(f);
  if (fwrite(&header, kWaveHeaderSize, 1, f) != 1) {
    perror(path);
    fclose(f);
    return;
  }
  if (fflush(f) != 0) {
    perror(path);
  }
  if (fclose(f) != 0) {
    perror(path);
  }
}

int main(int argc, char *argv[]) {
  for (int i = 1; i < argc; i++) {
    FixSampleRate(argv[i]);
  }
  return 0;
}
