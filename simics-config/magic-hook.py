import script_branch
import conf

MAGIC_NUMBER = 28
SANITIZER_SHM_ADDR = 0x20000000

def branch():
    while True:
        conf.bp.magic.cli_cmds.wait_for(number=MAGIC_NUMBER)
        print("asan: ", end="")
        addr = SANITIZER_SHM_ADDR
        while True:
            byte = conf.board.phys_mem.memory[addr]
            if byte == 0:
                break
            print(chr(byte), end="")
            if byte == 10:
                print("asan: ", end="")
            addr += 1
        print()

script_branch.sb_create(branch)
