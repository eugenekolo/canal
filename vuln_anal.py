import argparse

def main(fileName):


###############################################################################
# Main ~!
###############################################################################
if __name__ == '__main__':
    parser = argparse.ArgumentParser()

    parser.add_argument('-f', '--file', help="Required. Specify the encrypted file to decrypt.", 
      required=True)
    parser.add_argument('-o', '--out', help="Specify where you want the decrypted file to go, default=stdout", 
      default=sys.stdout)
    parser.add_argument('-x', '--keyblob', help="Required. Specify the keyblob, formated as a hex string. \
        Ex. 0802000010660000200000003310a5b100856.", required=True)
    args = parser.parse_args()
    cryptapi_decrypt(args.file, args.out, args.keyblob)

