lines = [x.split() for x in open("ray.dsl")]
mem = {}
p = 0

for w in lines:
    if w[0] != "var":
        continue
    n = w[1]
    mem[n] = {"at": p, "tmp": [p + 4, p + 8, p + 12, p + 16]}
    p += 20

out = []
here = 0


def go(p):
    global here
    d = p - here
    out.append((">" if d > 0 else "<") * abs(d))
    here = p


def clear(p):
    go(p)
    out.append("[-]")


def move(a, b):
    go(a)
    out.append("[-")
    go(b)
    out.append("+")
    go(a)
    out.append("]")


def split(a, b, c):
    go(a)
    out.append("[-")
    go(b)
    out.append("+")
    go(c)
    out.append("+")
    go(a)
    out.append("]")


def addcell(a, b, t, c):
    clear(t)
    clear(c)
    split(b, t, c)
    move(t, b)
    split(a, t, c)
    move(t, a)
    go(c)


def number(n):
    n = round(float(n) * 65536) & 0xFFFFFFFF
    return [(n >> i) & 255 for i in (0, 8, 16, 24)]


def clear4(n):
    a = mem[n]["at"]
    for i in range(4):
        clear(a + i)


def set4(n, v):
    a = mem[n]["at"]
    for i, b in enumerate(number(v)):
        go(a + i)
        out.append("[-]" + "+" * b)


for w in lines:
    if w[0] == "at":
        go(mem[w[1]]["at"] + (int(w[2]) if len(w) > 2 else 0))
    if w[0] == "zero":
        clear4(w[1])
    if w[0] == "set":
        set4(w[1], w[2])

open("ray.bf", "w").write("".join(out))

with open("ray.map", "w") as f:
    print("cells", p, file=f)
    for n in mem:
        print(n, mem[n], file=f)

print(len(mem), "vars", p, "cells", len("".join(out)), "characters")
