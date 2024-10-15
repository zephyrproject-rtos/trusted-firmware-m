import argparse

def pre_parse_args(name, help, parser):
    pre_arg_parser = argparse.ArgumentParser(add_help=False)

    pre_arg_parser.add_argument(name, help=help, required=True)
    parser.add_argument(name, help=help, required=True)

    parsed, _ = pre_arg_parser.parse_known_args()
    return getattr(parsed, name[2:])
