import sys

# Non-hermetic dependencies: apt-get install python-numpy python-matplotlib
import numpy
from matplotlib import colors
from matplotlib import pyplot

from sweet import voice_data_pb2

world_data = voice_data_pb2.WorldData.FromString(sys.stdin.read())
f0 = numpy.exp([f.lf0 for f in world_data.frame])
sp = numpy.array([list(f.sp) for f in world_data.frame]).transpose()

inf = 1e300 * 1e300
nan = inf - inf

fig, ax = pyplot.subplots(2, sharex=True)
ax[0].plot([x if x > 1 else nan for x in f0])
ax[1].matshow(sp, origin='lower', aspect='auto', interpolation='none',
              cmap='Greys', norm=colors.PowerNorm(0.15, sp.min(), sp.max()))
fig.subplots_adjust(hspace=0.25)

pyplot.show()
