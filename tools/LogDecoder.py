import argparse
from Decoder import Decoder
from GUI import GUI

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
                    prog='LogDecoder',
                    description='Decodes the logs created by the E220-900T22D for the STM32 MCUs.',
                    epilog='Written by @dabecart, 2025.')
    parser.add_argument('filename',
                        help="File location of the log file.")
    
    args = parser.parse_args()
    decoder = Decoder(args.filename)
    gui = GUI(decoder)

    gui.app.run(debug=False)
