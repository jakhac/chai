#pragma once

/// <summary>
/// Checks if index is square on board.
/// </summary>
/// <param name="sq">Square to check</param>
/// <returns>True if square is valid</returns>
int squareOnBoard(int sq);

/// <summary>
/// Checks if piece has valid index, where [1, 13] ist valid and zero is non valid.
/// </summary>
/// <param name="piece">Piece</param>
/// <returns>True if piece is valid</returns>
int pieceValid(int piece);
