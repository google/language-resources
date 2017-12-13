import argparse
import ConfigParser
import re
import sys


if __name__ == "__main__":

    conf = sys.argv[1]
    count = int(sys.argv[2])

    count -=4
    train = count/2
    valid = count/4
    test = count/4

    Config = ConfigParser.ConfigParser()
    Config.read(conf)

    # Setup data splits.
    splits = [
                    ["train_file_number", 2],
                    ["test_file_number",4], 
                    ["valid_file_number", 4]]

    map(lambda x: Config.set("Data", x[0], count/x[1]), splits)

    # Setup DNN architecture. 
    Config.set("Architecture", "training_epochs", 2)
    Config.set("Architecture", "warmup_epoch", 2)

    Config.write(sys.stdout)
