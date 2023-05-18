#!/usr/bin/env python
# -*- coding: UTF-8 -*-
'''
Agent Cezarego Czubały
'''


import copy
import math
import random
import sys


class Reversi:
    M = 8
    DIRS = [(0, 1), (1, 0), (-1, 0), (0, -1),
            (1, 1), (-1, -1), (1, -1), (-1, 1)]

    def __init__(self):
        self.board = self.initial_board()
        self.fields = set()
        self.move_list = []
        self.history = []
        for i in range(self.M):
            for j in range(self.M):
                if self.board[i][j] is None:
                    self.fields.add((j, i))

    def initial_board(self):
        B = [[None] * self.M for _ in range(self.M)]
        B[3][3] = 1
        B[4][4] = 1
        B[3][4] = 0
        B[4][3] = 0
        return B

    def draw(self):
        resString = ""
        for i in range(self.M):
            res = []
            for j in range(self.M):
                b = self.board[i][j]
                if b is None:
                    res.append('.')
                elif b == 1:
                    res.append('#')
                else:
                    res.append('o')
            resString += ''.join(res) + '\n'
        return resString

    def moves(self, player):
        res = []
        for (x, y) in self.fields:
            if any(self.can_beat(x, y, direction, player)
                   for direction in self.DIRS):
                res.append((x, y))
        return res

    def can_beat(self, x, y, d, player):
        dx, dy = d
        x += dx
        y += dy
        cnt = 0
        while self.get(x, y) == 1 - player:
            x += dx
            y += dy
            cnt += 1
        return cnt > 0 and self.get(x, y) == player

    def get(self, x, y):
        if 0 <= x < self.M and 0 <= y < self.M:
            return self.board[y][x]
        return None

    def do_move(self, move, player):
        assert player == len(self.move_list) % 2
        self.history.append([x[:] for x in self.board])
        self.move_list.append(move)

        if move is None:
            return
        x, y = move
        x0, y0 = move
        self.board[y][x] = player
        self.fields -= set([move])
        for dx, dy in self.DIRS:
            x, y = x0, y0
            to_beat = []
            x += dx
            y += dy
            while self.get(x, y) == 1 - player:
                to_beat.append((x, y))
                x += dx
                y += dy
            if self.get(x, y) == player:
                for (nx, ny) in to_beat:
                    self.board[ny][nx] = player

    def result(self):
        res = 0
        for y in range(self.M):
            for x in range(self.M):
                b = self.board[y][x]
                if b == 0:
                    res -= 1
                elif b == 1:
                    res += 1
        return res
    
    def heuristic_eval(self, player):
        max_player = 0
        min_player = 0

        max_player_corners = 0
        min_player_corners = 0

        for y in [0, self.M - 1]:
            for x in [0, self.M - 1]:
                if self.board[y][x] == player:
                    max_player_corners += 1
                elif self.board[y][x] == player:
                    min_player_corners += 1


        for y in range(self.M):
            for x in range(self.M):
                b = self.board[y][x]
                if b == player:
                    max_player += 1
                elif b == 1 - player:
                    min_player -= 1

        parity = 0
        
        if max_player + min_player > 0:
            parity = 100 * (max_player - min_player) / (max_player + min_player)

# mobility
        max_player_moves_count = len(self.moves(player))
        min_player_moves_count = len(self.moves(1 - player))

        mobility = 0

        if max_player_moves_count + min_player_moves_count > 0:
            mobility = 100 * (max_player_moves_count - min_player_moves_count) / (max_player_moves_count + min_player_moves_count)

# corners

        corners = 0

        if max_player_corners + min_player_corners > 0:
            corners = 100 * (max_player_corners - min_player_corners) / (max_player_corners + min_player_corners)

# corner closeness

        max_close_corners = 0
        min_close_corners = 0

        for y in [0, self.M - 1]:
            for x in [0, self.M - 1]:
                for dy in [-1, 0, 1]:
                    for dx in [-1, 0, 1]:
                        if dx == 0 and dy == 0:
                            continue

                        if self.get(x + dx, y + dy) == player:
                            max_close_corners += 1
                        elif self.get(x + dx, y + dy) == 1 - player:
                            min_close_corners += 1
        
        close_corners = 0

        if max_close_corners + min_close_corners > 0:
            close_corners = 100 * (max_close_corners - min_close_corners) / (max_close_corners + min_close_corners)

# front tiles

        max_front_tiles = 0
        min_front_tiles = 0

        for y in [0, self.M - 1]:
            for x in [0, self.M - 1]:
                for dy in [-1, 0, 1]:
                    if self.get(x, y) != None:
                        continue

                    for dx in [-1, 0, 1]:
                        if dx == 0 and dy == 0:
                            continue

                        if self.get(x + dx, y + dy) == player:
                            max_front_tiles += 1
                        elif self.get(x + dx, y + dy) == 1 - player:
                            min_front_tiles += 1
        
        front_tiles = 0

        if max_front_tiles + min_front_tiles > 0:
            front_tiles = 100 * (max_front_tiles - min_front_tiles) / (max_front_tiles + min_front_tiles)

# weighted tiles
        weights = [
            [20, -3, 11, 8, 8, 11, -3, 20],
            [-3, -7, -4, 1, 1, -4, -7, -3],
            [11, -4, 2, 2, 2, 2, -4, 11],
            [8, 1, 2, -3, -3, 2, 1, 8],
            [8, 1, 2, -3, -3, 2, 1, 8],
            [11, -4, 2, 2, 2, 2, -4, 11],
            [-3, -7, -4, 1, 1, -4, -7, -3],
            [20, -3, 11, 8, 8, 11, -3, 20]
        ]

        max_weighted_tiles = 0
        min_weighted_tiles = 0

        for y in [0, self.M - 1]:
            for x in [0, self.M - 1]:
                if self.get(x, y) == player:
                    max_weighted_tiles += weights[y][x]
                elif self.get(x, y) == 1 - player:
                    min_weighted_tiles += weights[y][x]

        weighted_tiles = 0
        
        if max_weighted_tiles + min_weighted_tiles > 0:
            weighted_tiles = 100 * (max_weighted_tiles - min_weighted_tiles) / (max_weighted_tiles + min_weighted_tiles)
        score = (10 * parity) + (801.724 * corners) + (382.026 * close_corners) + (78.922 * mobility) + (74.396 * front_tiles) + (10 * weighted_tiles)

        return score

    def terminal(self):
        if not self.fields:
            return True
        if len(self.move_list) < 2:
            return False
        return self.move_list[-1] is None and self.move_list[-2] is None


class Player(object):
    def __init__(self):
        self.reset()

    def reset(self):
        self.game = Reversi()
        self.my_player = 1
        self.say('RDY')

    def say(self, what):
        sys.stdout.write(what)
        sys.stdout.write('\n')
        sys.stdout.flush()

    def hear(self):
        line = sys.stdin.readline().split()
        return line[0], line[1:]

    @staticmethod
    def min_max(currentBoard, depth, maximizingPlayer, player, orPlayer, alpha, beta):
        
        if depth == 0 or currentBoard.terminal():
            return currentBoard.heuristic_eval(orPlayer)
        
        if maximizingPlayer:
            maxEval = -math.inf
            moves = currentBoard.moves(player)

            for move in moves:
                newBoard = copy.deepcopy(currentBoard)
                newBoard.do_move(move, player)
                maxEval = max(maxEval, Player.min_max(newBoard, depth - 1, not maximizingPlayer, 1 - player, orPlayer, alpha, beta))
                alpha = max(maxEval, alpha)

                if beta <= alpha:
                    break

            return maxEval
        else:
            minEval = math.inf
            moves = currentBoard.moves(player)

            for move in moves:
                newBoard = copy.deepcopy(currentBoard)
                newBoard.do_move(move, player)
                minEval = min(minEval, Player.min_max(newBoard, depth - 1, not maximizingPlayer, 1 - player, orPlayer, alpha, beta))
                beta = min(minEval, beta)

                if beta <= alpha:
                    break
            return minEval

    def loop(self):
        CORNERS = { (0,0), (0,7), (7,0), (7,7)}
        while True:
            cmd, args = self.hear()
            if cmd == 'HEDID':
                unused_move_timeout, unused_game_timeout = args[:2]
                move = tuple((int(m) for m in args[2:]))
                if move == (-1, -1):
                    move = None
                self.game.do_move(move, 1 - self.my_player)
            elif cmd == 'ONEMORE':
                self.reset()
                continue
            elif cmd == 'BYE':
                break
            else:
                assert cmd == 'UGO'
                assert not self.game.move_list
                self.my_player = 0

            moves = self.game.moves(self.my_player)

            # sys.stderr.write(self.game.draw())
            # sys.stderr.write("\n")
            # sys.stderr.flush()

            if moves:
                bestMove = moves[0]
                bestBoard = copy.deepcopy(self.game)

                for move in moves:
                    if move == bestMove:
                        continue

                    newboard = copy.deepcopy(self.game)
                    newboard.do_move(move, self.my_player)
                    
                    if newboard.terminal():
                        bestMove = move
                        break

                    if bestBoard.heuristic_eval(self.my_player) < Player.min_max(newboard, 2, False, 1 - self.my_player, self.my_player, -math.inf, math.inf):
                        bestMove = move
                        bestBoard = newboard

                self.game.do_move(bestMove, self.my_player)
                move = bestMove
            else:
                self.game.do_move(None, self.my_player)
                move = (-1, -1)


            self.say('IDO %d %d' % move)


if __name__ == '__main__':
    player = Player()
    player.loop()
