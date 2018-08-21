import os, sys

# this code takes input one folder full of integer symbol FSTs and converts it to ascii symbol FSTs , using a reference ASCII symbol table.

if len(sys.argv) != 4:
    print 'usage: python dump_asciifst.py <ascii syms table> <raw fsts in-dir> <ascii fsts out-dir>'
    sys.exit()

ascii_syms = sys.argv[1]
indir = sys.argv[2]
outdir = sys.argv[3]

os.system("mkdir -p "+outdir)
for f in os.listdir(indir):
  os.system("fstprint --isymbols="+ascii_syms+" --osymbols="+ascii_syms+" "+indir+"/"+f+ " > "+outdir+"/"+f)
