import sys
import Logger

RED_BOLD = "\033[1;31m"
RESET_COLOR = "\033[0m"

def putMsg(message):
	print(RED_BOLD + "ERROR" + RESET_COLOR + ':', message, file = sys.stderr)
	Logger.log(message + "\n")

def exitError(message):
	putMsg(message)
	exit(1)
