## <img src="https://i.imgur.com/Py8am6G.png" width="30%" height="30%"/>
Integral is a [UCI-compliant](https://en.wikipedia.org/wiki/Universal_Chess_Interface) chess engine developed in C++. It is a personal project developed with the goal of meshing my admiration for chess with programming. \
\
**Play against Integral on [Lichess](https://lichess.org/@/IntegralBot)**

## Usage
Integral supports the following UCI commands:
- `uci` Prints information about Integral and replies with `uciok`
- `ucinewgame` Sets up a new game and clears the transposition table
- `isready` Replies with `readyok`
- `position startpos` Sets the board state and pieces to the starting position
- `position fen <string>` Sets the board state and pieces to the given [FEN](https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation) string
- `position fen <string> moves <e2e4 e7e5 ...>` Plays the given moves from the FEN position
- `go depth <depth>` Searches up to the given depth and replies with `bestmove <move>`
- `go infinite` Searches up to the maximum search depth (100) and replies with `bestmove <move>`
- `go wtime <time> btime <time> winc <inc> binc <inc>` Searches for and replies with the best move given within the time/increment allotted. The amount of time used is managed by an internal time management system to ensure the engine doesn't run out of time.
- `go movetime <time>` Searches for the best move using the full time allotted.

## Compilation
> [!NOTE]  
> Integral must be compiled with either GCC v13 (or higher) or Clang v15 (or higher)

To compile Integral, enter the following commands in a terminal:
```shell
git clone https://github.com/aronpetko/integral
cd integral
cmake .
make
```

## Rating
Integral is estimated to be around 2700 [CCRL](https://www.computerchess.org.uk/ccrl/) Blitz. Unfortunately, there is no accurate way to translate chess engine ratings to human ratings. A very rough estimate would be that Integral can consistently beat 2400 FIDE-rated players.