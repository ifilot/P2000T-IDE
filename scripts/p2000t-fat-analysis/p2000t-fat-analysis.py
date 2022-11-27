# -*- coding: utf-8 -*-

import colorama
from colorama import Fore, Style

def main():
    f = open('tape01-06.bin', 'rb')
    data = bytearray(f.read())
    f.close()
    
    # scan bank metadata sections for start blocks
    startblocks = scan_bank_metadata_blocks(data)
    
    for startblock in startblocks:
        parse_file(data, startblock[0], startblock[1])
    
def scan_bank_metadata_blocks(data):
    startblocks = []
    for i in range(0,8):
        bankdata = data[i*0x10000:i*0x10000+0x100]
        for j in bankdata:
            if j != 0xFF:
                startblocks.append((i,j))
    
    return startblocks

def print_metadata(metadata, blocklist, valid=True):
    
    blocklistformatted = "->".join(["%02i.%02i" % x for x in blocklist])
    
    validflag = ""
    if valid:
        validflag = Fore.GREEN + "✓" + Style.RESET_ALL
    else:
        validflag = Fore.RED + "✗" + Style.RESET_ALL
    
    print("%s | %s | %s | %02i | %s" % \
          (metadata['filename'],
           metadata['extension'],
           validflag,
           metadata['total_blocks'],
           blocklistformatted))

def parse_file(data, bank=0, block=0):
    nrblocks = 0
    
    # track all addresses
    blocks = []
    
    # start reading metadata of first bank/block
    metadata = readmetadata(data, bank, block)
    blocks.append((bank,block))
    
    if not metadata:
        return 0xFF, 0xFF
    
    # block counter
    nrblocks += 1
    
    # placeholder
    lastbank = bank
    lastblock = block
    
    while metadata['next_bank'] != 0xFF and metadata['next_block'] != 0xFF:
        nrblocks += 1
        lastbank = metadata['next_bank']
        lastblock = metadata['next_block']
        blocks.append((lastbank,lastblock))
        metadata = readmetadata(data, 
                                metadata['next_bank'], 
                                metadata['next_block'])
    
    if metadata['total_blocks'] != nrblocks:
        print_metadata(metadata,blocks,valid=False)
    else:
        print_metadata(metadata,blocks)    
    
    # returning last bank and block
    return lastbank, lastblock

def readmetadata(data, bank=0, block=0):   
    """
    Read metadata at bank and block ids
    """
    # set block address
    start = 0x100 + block * 0x40 + bank*0x10000
    
    # check marker byte
    if data[start + 8] != 0x00:
        return None
    
    # read descriptor
    filename = data[start+0x26:start+0x2E]+data[start+0x37:start+0x3E]
    filename = filename.decode('utf8')
    
    # read current bank
    current_bank = data[start]
    
    # read rom address
    rom_address = data[start+1] + 256 * data[start+2]
    
    # read current block
    current_block = data[start+9]
    
    # read current block
    total_blocks = data[start+10]
    
    # file type extension
    extension = data[start+0x2E:start+0x2E+3].decode('utf8')
    
    # read next bank
    next_bank = data[start+3]
    
    # read next address
    next_block = data[start+4]

    metadata = {
        'filename' : filename,
        'extension': extension,
        'current_bank'  : current_bank,
        'rom_address'   : rom_address,
        'current_block' : current_block,
        'total_blocks'  : total_blocks,
        'next_bank'     : next_bank,
        'next_block'    : next_block
    }

    return metadata
    
if __name__ == '__main__':
    main()