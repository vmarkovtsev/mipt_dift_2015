import sys

def main():
    required = [42, 111540, 1333, 98, 166, 1991, 160, 161, 162, 163, 2001, 2004, 3409, 6, 1087, 3494, 95, 1332, 2598, 178, 180, 1995, 524, 185, 606, 1377, 2786, 1362, 1376, 111689, 111690, 3586]
    req_set = set(required)
    solved = {int(l.strip()) for l in sys.stdin.read().split('\n') if l.strip()}
    if len(sys.argv) > 1:
        print("Overall: %d" % len(solved))
        not_solved = 0
        for r in req_set:
            if r not in solved:
                print(r)
                not_solved += 1
        print("Not solved: %d" % not_solved)
        for r in req_set:
            if r in solved:
                print(r)
    for r in required:
        sys.stdout.write("1\t" if r in solved else "0\t")
    print()


if __name__ == "__main__":
    main()
