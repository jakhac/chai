import chess
import optparse
import linecache
import sys

parser = optparse.OptionParser()

parser.add_option('-d', '--depth', action="store", dest="depth", type=int, help="search depth", default=1)
parser.add_option('-f', '--fen', action="store", dest="fen", help="fen board", default="rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")
options, args = parser.parse_args()

print("Depth:", options.depth)
print('FEN:', options.fen)

# vars
board = chess.Board(fen=options.fen)
counter = 0
depth = options.depth

def perftRoot(d):

    print("Start perft")
    
    legal_move_list = list(board.legal_moves)
    moveNum = 0
    for move in legal_move_list:
        board.push(move)

        cumNodes = counter
        perft(board, d - 1)

        board.pop()

        oldNodes = counter - cumNodes
        # print("Move:", move, " : ", oldNodes)
        print(move, ":", oldNodes)

    print("Found", counter, "nodes at search depth", d)
    return


def perft(board, d):
    global counter

    if d == 0:
        counter += 1
        return
    
    legal_move_list = list(board.legal_moves)
    for move in legal_move_list:
        board.push(move)
        perft(board, d - 1)
        board.pop()

    return


def getline(stream, delimiter="\n"):
  def _gen():
    while 1:
      line = stream.readline()
      if delimiter in line:
        yield line[0:line.index(delimiter)]
        break
      else:
        yield line
  return "".join(_gen())


def main():
  global depth, counter

  while depth:
    perftRoot(depth)
    depth -= 1
    counter = 0
    print("\nDivide move at:", "")
    move = getline(sys.stdin)
    uci_move = chess.Move.from_uci(move)
    # board.push_san(move)
    board.push(uci_move)
    print("Divided at move", move)


  return



if __name__ == "__main__":
    main()
    # while(True):
        # line_number = getline(sys.stdin)
        # print()



