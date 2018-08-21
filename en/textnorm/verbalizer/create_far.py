import os, sys

if len(sys.argv) != 5:
    print 'usage: python create_far.py <ascii syms table> <symbol fsts in-dir> <work tmp dir> <out-far>'
    sys.exit()

ascii_syms = sys.argv[1]
indir = sys.argv[2]
workdir = sys.argv[3]
outfar = sys.argv[4]

os.system("mkdir -p "+workdir)
for f in os.listdir(indir):
  os.system("fstcompile --isymbols="+ascii_syms+" --osymbols="+ascii_syms+" "+indir+"/"+f+ " > "+workdir+"/"+f)

os.system("farcreate "+ workdir + "/* " + outfar)
#os.system("rm -r " + workdir)
