#! /usr/bin/env python

"""Pretty print a byte stream suspected to be UTF-8 text.

This should work with most versions of Python on Posix systems. In particular
it works with CPython 2.6, CPython 2.7, and CPython 3.3+, on Linux and Darwin,
with narrow (16 bits per character) and wide (21 or more bits per character)
Unicode strings, and with the native unicodedata module or the icu module
provided by PyICU.
"""

from __future__ import unicode_literals

import io
import sys

try:
  import icu  # pylint: disable=g-import-not-at-top

  INFO = 'Unicode %s (ICU %s) via module %s\n' % (icu.UNICODE_VERSION,
                                                  icu.ICU_VERSION,
                                                  icu.__name__)

  _NFD = icu.Normalizer2.getNFDInstance()

  def IsPrintable(uchr):
    return icu.Char.isprint(uchr)

  def IsFormat(uchr):
    return icu.Char.charType(uchr) == icu.UCharCategory.FORMAT_CHAR

  def CharName(uchr):
    return icu.Char.charName(uchr)

  def ToNFD(uchr):
    return _NFD.normalize(uchr)


except ImportError:
  import unicodedata  # pylint: disable=g-import-not-at-top

  INFO = 'Unicode %s via module %s\n' % (unicodedata.unidata_version,
                                         unicodedata.__name__)

  def IsPrintable(uchr):
    return not unicodedata.category(uchr).startswith('C')

  def IsFormat(uchr):
    return unicodedata.category(uchr) == 'Cf'

  def CharName(uchr):
    return unicodedata.name(uchr, '')

  def ToNFD(uchr):
    return unicodedata.normalize('NFD', uchr)


_STDOUT = io.open(1, mode='wt', encoding='utf-8', closefd=False)
_STDERR = io.open(2, mode='wt', encoding='utf-8', closefd=False)

CHAR_ESCAPE = {
    0x00: r'\0',
    0x07: r'\a',
    0x08: r'\b',
    0x09: r'\t',
    0x0A: r'\n',
    0x0B: r'\v',
    0x0C: r'\f',
    0x0D: r'\r',
}

CHAR_NAME = {
    # C0 controls
    0x00: 'NULL',
    0x01: 'START OF HEADING',
    0x02: 'START OF TEXT',
    0x03: 'END OF TEXT',
    0x04: 'END OF TRANSMISSION',
    0x05: 'ENQUIRY',
    0x06: 'ACKNOWLEDGE',
    0x07: 'BELL',
    0x08: 'BACKSPACE',
    0x09: 'CHARACTER TABULATION',
    0x0A: 'LINE FEED (LF)',
    0x0B: 'LINE TABULATION',
    0x0C: 'FORM FEED (FF)',
    0x0D: 'CARRIAGE RETURN (CR)',
    0x0E: 'SHIFT OUT',
    0x0F: 'SHIFT IN',
    0x10: 'DATA LINK ESCAPE',
    0x11: 'DEVICE CONTROL ONE',
    0x12: 'DEVICE CONTROL TWO',
    0x13: 'DEVICE CONTROL THREE',
    0x14: 'DEVICE CONTROL FOUR',
    0x15: 'NEGATIVE ACKNOWLEDGE',
    0x16: 'SYNCHRONOUS IDLE',
    0x17: 'END OF TRANSMISSION BLOCK',
    0x18: 'CANCEL',
    0x19: 'END OF MEDIUM',
    0x1A: 'SUBSTITUTE',
    0x1B: 'ESCAPE',
    0x1C: 'INFORMATION SEPARATOR FOUR (FILE)',
    0x1D: 'INFORMATION SEPARATOR THREE (GROUP)',
    0x1E: 'INFORMATION SEPARATOR TWO (RECORD)',
    0x1F: 'INFORMATION SEPARATOR ONE (UNIT)',
    0x7F: 'DELETE',
    # C1 controls
    0x80: 'XXX',
    0x81: 'XXX',
    0x82: 'BREAK PERMITTED HERE',
    0x83: 'NO BREAK HERE',
    0x84: 'INDEX',
    0x85: 'NEXT LINE (NEL)',
    0x86: 'START OF SELECTED AREA',
    0x87: 'END OF SELECTED AREA',
    0x88: 'CHARACTER TABULATION SET',
    0x89: 'CHARACTER TABULATION WITH JUSTIFICATION',
    0x8A: 'LINE TABULATION SET',
    0x8B: 'PARTIAL LINE FORWARD',
    0x8C: 'PARTIAL LINE BACKWARD',
    0x8D: 'REVERSE LINE FEED',
    0x8E: 'SINGLE SHIFT TWO',
    0x8F: 'SINGLE SHIFT THREE',
    0x90: 'DEVICE CONTROL STRING',
    0x91: 'PRIVATE USE ONE',
    0x92: 'PRIVATE USE TWO',
    0x93: 'SET TRANSMIT STATE',
    0x94: 'CANCEL CHARACTER',
    0x95: 'MESSAGE WAITING',
    0x96: 'START OF GUARDED AREA',
    0x97: 'END OF GUARDED AREA',
    0x98: 'START OF STRING',
    0x99: 'XXX',
    0x9A: 'SINGLE CHARACTER INTRODUCER',
    0x9B: 'CONTROL SEQUENCE INTRODUCER',
    0x9C: 'STRING TERMINATOR',
    0x9D: 'OPERATING SYSTEM COMMAND',
    0x9E: 'PRIVACY MESSAGE',
    0x9F: 'APPLICATION PROGRAM COMMAND',
}


def IsHighSurrogateCodepoint(cp):
  return 0xD800 <= cp <= 0xDBFF


def IsLowSurrogateCodepoint(cp):
  return 0xDC00 <= cp <= 0xDFFF


def SurrogatePayload(cp):
  payload = cp & ((1 << 10) - 1)
  if IsHighSurrogateCodepoint(cp):
    return 0x10000 + (payload << 10)
  elif IsLowSurrogateCodepoint(cp):
    return payload
  raise ValueError('SurrogatePayload() arg is not a surrogate: %X' % cp)


def CharToCodepoint(char):
  if len(char) == 1:
    return ord(char)
  elif len(char) == 2:
    hi = ord(char[0])
    lo = ord(char[1])
    if IsHighSurrogateCodepoint(hi) and IsLowSurrogateCodepoint(lo):
      return SurrogatePayload(hi) | SurrogatePayload(lo)
  raise TypeError('CharToCodepoint expected a character or surrogate pair')


def CodepointToChar(codepoint):
  return ('\\U%08X' % codepoint).encode('ascii').decode('unicode-escape')


def EscapeCodepoint(cp):
  if cp in CHAR_ESCAPE:
    return CHAR_ESCAPE[cp]
  elif cp <= 0xFF:
    return '\\x%02x' % cp
  elif cp <= 0xFFFF:
    return '\\u%04x' % cp
  else:
    return '\\U%08x' % cp


def CodepointName(cp):
  """Return a printable name for the given codepoint."""
  name = CHAR_NAME.get(cp, '')
  if name:
    return name
  if (0xE000 <= cp <= 0xF8FF or
      0xF0000 <= cp <= 0xFFFFD or
      0x100000 <= cp <= 0x10FFFD):
    return '<Private Use>'
  elif IsHighSurrogateCodepoint(cp):
    return '<%sPrivate Use High Surrogate %X>' % (
        'Non ' if cp <= 0xDB7F else '', SurrogatePayload(cp))
  elif IsLowSurrogateCodepoint(cp):
    return '<Low Surrogate %03X>' % SurrogatePayload(cp)
  elif 0xFDD0 <= cp <= 0xFDEF or (cp & 0xFFFF) >= 0xFFFE:
    return '<Noncharacter>'
  return ''


def Decomposition(uchr):
  nfd = ToNFD(uchr)
  if nfd == uchr:
    return ''
  return ' '.join('%04X' % ord(c) for c in nfd)


def Write(writer, msg, *args):
  writer.write(msg % tuple(args))
  writer.flush()
  return


def IsFollowByte(byte):
  return 0b10000000 <= byte <= 0b10111111


def Utf8ChunkedStream(bstream):
  """Yield putative UTF-8 byte sequences of lead byte and follow bytes."""
  utf8_bytes = []
  while True:
    bstring = bstream.read(1)
    if not bstring:
      # EOF reached.
      if utf8_bytes:
        yield utf8_bytes
      return
    assert len(bstring) == 1
    byte = ord(bstring)
    if IsFollowByte(byte):
      utf8_bytes.append(byte)
    else:
      if utf8_bytes:
        yield utf8_bytes
      utf8_bytes = [byte]
  return


def FormatByteSequence(byte_sequence):
  return '%s (%s)' % (
      ' '.join('{:02X}'.format(b) for b in byte_sequence),
      ' '.join('{:08b}'.format(b) for b in byte_sequence),
  )


def CodepointStream(bstream, writer):
  """Decoder for Thompson and Pike's FSS-UTF encoding that yields codepoints."""
  for utf8_bytes in Utf8ChunkedStream(bstream):
    assert utf8_bytes
    byte = utf8_bytes[0]
    if 0 <= byte <= 0b01111111:
      lead_payload = byte
      min_payload = 0
      utf8_length = 1
    elif 0b11000000 <= byte <= 0b11011111:
      lead_payload = byte & 0b00011111
      min_payload = 0x80
      utf8_length = 2
    elif 0b11100000 <= byte <= 0b11101111:
      lead_payload = byte & 0b00001111
      min_payload = 0x800
      utf8_length = 3
    elif 0b11110000 <= byte <= 0b11110111:
      lead_payload = byte & 0b00000111
      min_payload = 0x10000
      utf8_length = 4
    elif 0b11111000 <= byte <= 0b11111011:
      lead_payload = byte & 0b00000011
      min_payload = 0x200000
      utf8_length = 5
    elif 0b11111100 <= byte <= 0b11111101:
      lead_payload = byte & 0b00000001
      min_payload = 0x4000000
      utf8_length = 6
    elif 0b11111110 <= byte:
      Write(writer, 'Error: Invalid UTF-8 sequence %s: '
            'lead byte too large\n',
            FormatByteSequence(utf8_bytes))
      continue
    else:
      assert IsFollowByte(byte)
      Write(writer, 'Error: Invalid UTF-8 sequence %s: '
            'first byte is a follow byte\n',
            FormatByteSequence(utf8_bytes))
      continue
    if len(utf8_bytes) != utf8_length:
      Write(writer, 'Error: Invalid UTF-8 sequence %s: '
            'expected %d bytes but found %d\n',
            FormatByteSequence(utf8_bytes), utf8_length, len(utf8_bytes))
      continue
    if utf8_length > 4:
      Write(writer, 'Warning: Unexpected UTF-8 sequence %s: '
            'expected at most 4 bytes but found %d\n',
            FormatByteSequence(utf8_bytes), utf8_length)
    payload = lead_payload
    for follow_byte in utf8_bytes[1:]:
      payload <<= 6
      payload |= follow_byte & 0b00111111
    assert 0 <= payload <= 0x7FFFFFFF
    if payload < min_payload:
      Write(writer, 'Warning: Unexpected UTF-8 sequence %s: '
            'overlong encoding of payload %X\n',
            FormatByteSequence(utf8_bytes), payload)
    if IsHighSurrogateCodepoint(payload) or IsLowSurrogateCodepoint(payload):
      Write(writer, 'Warning: Unexpected UTF-8 sequence %s: '
            'surrogate codepoint %X encoded as UTF-8\n',
            FormatByteSequence(utf8_bytes), payload)
    yield payload
  return


def Dump(bstream):
  """Dump information about a byte stream suspected to be UTF-8."""
  for codepoint in CodepointStream(bstream, _STDERR):
    if 0 <= codepoint <= 0x10FFFF:
      # First, work around platform/PyICU bugs in handling surrogates:
      if (IsHighSurrogateCodepoint(codepoint) or
          IsLowSurrogateCodepoint(codepoint)):
        printable = EscapeCodepoint(codepoint)
        name = CodepointName(codepoint)
        deco = ''
      else:
        char = CodepointToChar(codepoint)
        if IsPrintable(char):
          printable = char
        else:
          printable = EscapeCodepoint(codepoint)
        name = CharName(char)
        if not name:
          name = CodepointName(codepoint)
        deco = Decomposition(char)
      Write(_STDOUT, '%s\t%s\t%s\t%s\n',
            printable, ('%04X' % codepoint).rjust(6), name, deco)
    else:
      Write(_STDERR, '\t%X\t<Invalid Codepoint>\n', codepoint)
  return


def SanityCheck(writer):
  Write(writer, INFO)
  if CharName('\U00010300') != 'OLD ITALIC LETTER A':
    Write(writer, 'Warning: Unicode data too old (< 3.1.0, 2001) or broken\n')
  return


def main(argv):
  SanityCheck(_STDERR)
  if len(argv) == 1:
    # For interactive use, read line by line from stdin:
    with io.open(0, mode='rb', buffering=0, closefd=False) as bstream:
      for line in bstream:
        Dump(io.BytesIO(line))
  else:
    for path in argv[1:]:
      with io.open(path, mode='rb') as bstream:
        Dump(bstream)
  return


if __name__ == '__main__':
  main(sys.argv)
