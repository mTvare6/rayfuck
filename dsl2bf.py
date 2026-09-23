lines = [x.split() for x in open("ray.dsl")]
mem = {}
p = 0

for w in lines:
    if w[0] != "var":
        continue
    n = w[1]
    mem[n] = {"at": p, "tmp": [p + 4, p + 8, p + 12, p + 16, p + 20, p + 24]}
    p += 28

out = []
here = 0


def go(ax):
    global here
    d = ax - here
    out.append((">" if d > 0 else "<") * abs(d))
    here = ax


def clear1(ax):
    go(ax)
    out.append("[-]")


# (src, dst)
# no clear at dst
def move1(ax, ay):
    go(ax)
    out.append("[-")
    go(ay)
    out.append("+")
    go(ax)
    out.append("]")


def copy2_by_delete(ax, ay, az):
    go(ax)
    out.append("[-")
    go(ay)
    out.append("+")
    go(az)
    out.append("+")
    go(ax)
    out.append("]")


def add1(ax, ay, at, ar):
    clear1(at)
    clear1(ar)
    copy2_by_delete(ay, at, ar)
    move1(at, ay)
    copy2_by_delete(ax, at, ar)
    move1(at, ax)
    go(ar)


def set1(ax, v):
    go(ax)
    out.append("[-]" + "+" * v)


# (src, dst)
def copy1(ax, ay, at):
    if ax == ay:
        return
    clear1(ay)
    clear1(at)
    copy2_by_delete(ax, ay, at)
    move1(at, ax)


def copyn(ax, ay, size, at):
    for i in range(size):
        # safely not used at at
        copy1(ax[i], ay[i], at)


def setn(ax, v):
    for a, n in zip(ax, v):
        set1(a, n)


def number(v):
    v = round(float(v) * 65536) & 0xFFFFFFFF
    return [(v >> i) & 255 for i in (0, 8, 16, 24)]


def cells(ax, size=4):
    return list(range(ax, ax + size))


def load(vx, ax, at):
    if vx in mem:
        # var
        copyn(cells(mem[vx]["at"]), ax, 4, at)
    else:
        setn(ax, number(vx))


def inc1carry(ax, az, af, ac=None):
    go(ax)
    out.append("+")
    if ac is None:
        return
    clear1(az)
    clear1(af)
    go(af)
    out.append("+")
    go(ax)
    out.append("[-")
    go(az)
    out.append("+")
    # not dec no underflow
    clear1(af)
    go(ax)
    out.append("]")
    move1(az, ax)
    move1(af, ac)


def incn(ax, az, af, ac, ad):
    clear1(ac)
    clear1(ad)
    inc1carry(ax[0], az, af, ac)
    carry = ac
    for i in range(1, len(ax)):
        nxt = ad if carry == ac else ac
        clear1(nxt)
        go(carry)
        out.append("[-")
        inc1carry(ax[i], az, af, nxt if i + 1 < len(ax) else None)
        go(carry)
        out.append("]")
        carry = nxt

# -x = ~x + 1
def negn(ax, az, af, ac, ad):
    # store 255 - x as comp(BF[a])
    for a in ax:
        set1(az, 255)
        go(a)
        out.append("[-")
        go(az)
        out.append("-")
        go(a)
        out.append("]")
        move1(az, a)
    incn(ax, az, af, ac, ad)

def addn(ax, ay, az, af, ac, ad):
    clear1(ac)
    clear1(ad)
    carry = ac
    # count 5 used in div
    for i in range(len(ax)):
        # two carry
        nxt = ad if carry == ac else ac
        clear1(nxt)
        if i:
            go(carry)
            out.append("[-")
            inc1carry(ax[i], az, af, nxt)
            go(carry)
            out.append("]")
        # destructive here
        go(ay[i])
        out.append("[-")
        inc1carry(ax[i], az, af, nxt)
        go(ay[i])
        out.append("]")
        carry = nxt
    return carry


#boolean
def bool_flip(ax, at):
    set1(at, 1)
    go(ax)
    out.append("[-")
    clear1(at)
    go(ax)
    out.append("]")
    move1(at, ax)


def extract_sign(ax, ar, aw):
    copy1(ax, aw[0], aw[2])
    set1(aw[1], 128)
    carry = addn([aw[1]], [aw[0]], aw[2], aw[3], aw[4], aw[5])
    clear1(ar)
    move1(carry, ar)


def truthiness(ax, ar):
    clear1(ar)
    for a in ax:
        go(a)
        out.append("[[-]")
        clear1(ar)
        go(ar)
        out.append("+")
        go(a)
        out.append("]")


def add4(nr, vx, vy, minus=False):
    a = cells(mem[nr]["at"])
    t = mem[nr]["tmp"]
    b = cells(t[0])
    c = cells(t[1])
    w = cells(t[2])
    load(vx, b, t[5])
    load(vy, c, t[5])
    if minus:
        negn(c, w[0], w[1], w[2], w[3])
    for i in range(4):
        clear1(a[i])
        move1(b[i], a[i])
    addn(a, c, w[0], w[1], w[2], w[3])
    go(a[0])


def neg4(nr, vx):
    a = cells(mem[nr]["at"])
    t = mem[nr]["tmp"]
    b = cells(t[0])
    w = cells(t[1])
    load(vx, b, t[5])
    negn(b, w[0], w[1], w[2], w[3])
    for i in range(4):
        clear1(a[i])
        move1(b[i], a[i])
    go(a[0])


def bool4(nr, vx, vy=None, both=False):
    a = cells(mem[nr]["at"])
    t = mem[nr]["tmp"]
    b = cells(t[0])
    load(vx, b, t[5])
    if vy is None:
        truthiness(b, t[2])
        clear1(a[0])
        move1(t[2], a[0])
        bool_flip(a[0], t[2])
    else:
        c = cells(t[1])
        load(vy, c, t[5])
        truthiness(b, t[2])
        truthiness(c, t[2] + 1)
        clear1(a[0])
        if both:
            go(t[2])
            out.append("[-")
            go(t[2] + 1)
            out.append("[-")
            go(a[0])
            out.append("+")
            go(t[2] + 1)
            out.append("]")
            go(t[2])
            out.append("]")
        else:
            truthiness([t[2], t[2] + 1], a[0])
    for p in a[1:]:
        clear1(p)
    go(a[0])


def compare1(ax, ay, al, ag, aw):
    aq, at, af, ai = aw
    clear1(al)
    clear1(ag)
    go(ax)
    out.append("[-")
    copy1(ay, aq, at)
    truthiness([aq], af)
    set1(ai, 1)
    go(af)
    out.append("[-")
    go(ay)
    out.append("-")
    clear1(ai)
    go(af)
    out.append("]")
    go(ai)
    out.append("[-")
    set1(ag, 1)
    clear1(ax)
    go(ai)
    out.append("]")
    go(ax)
    out.append("]")
    truthiness([ay], al)






def shift1(ax, ac, aw):
    aq, at, az, af, aa, ab = aw
    clear1(ac)
    for x in ax:
        copy1(x, aq, at)
        carry = addn([x], [aq], az, af, aa, ab)
        go(ac)
        out.append("[-")
        go(x)
        out.append("+")
        go(ac)
        out.append("]")
        move1(carry, ac)


def lessn(ax, ay, al, ae, ab, aw):
    abl, abg = ab
    axc, ayc, at, ar = aw[:4]
    clear1(al)
    set1(ae, 1)
    for i in range(len(ax) - 1, -1, -1):
        copy1(ae, ar, at)
        clear1(ae)
        go(ar)
        out.append("[-")
        copy1(ax[i], axc, at)
        copy1(ay[i], ayc, at)
        compare1(axc, ayc, abl, abg, aw[4:])
        set1(ae, 1)
        go(abl)
        out.append("[-")
        set1(al, 1)
        clear1(ae)
        go(abl)
        out.append("]")
        go(abg)
        out.append("[-")
        clear1(ae)
        go(abg)
        out.append("]")
        go(ar)
        out.append("]")


def sqrt4(nr, vx):
    a = cells(mem[nr]["at"])
    t = mem[nr]["tmp"]
    b = cells(t[0])
    rem = cells(t[1])
    root = cells(t[2])
    trial = cells(t[3])
    w = cells(t[4]) + cells(t[5])
    load(vx, b, w[7])
    for x in rem + root + trial:
        clear1(x)
    for pair in range(24):
        for bit in range(2):
            shift1(rem, a[1], w[:6])
            if pair < 16:
                shift1(b, a[0], w[:6])
                move1(a[0], rem[0])
        shift1(root, a[0], w[:6])
        copyn(root, trial, 4, w[0])
        shift1(trial, a[0], w[:6])
        go(trial[0])
        out.append("+")
        lessn(rem, trial, a[0], a[3], [a[1], a[2]], w)
        bool_flip(a[0], w[0])
        go(a[0])
        out.append("[-")
        negn(trial, w[0], w[1], w[2], w[3])
        addn(rem, trial, w[0], w[1], w[2], w[3])
        go(root[0])
        out.append("+")
        go(a[0])
        out.append("]")
    for i in range(4):
        clear1(a[i])
        move1(root[i], a[i])
    go(a[0])




def compare4(nr, vx, vy, op):
    a = cells(mem[nr]["at"])
    t = mem[nr]["tmp"]
    b = cells(t[0])
    c = cells(t[1])
    al, ag, more, run = cells(t[2])
    w = cells(t[3])
    addw = cells(t[4])
    load(vx, b, t[5] + 3)
    load(vy, c, t[5] + 3)
    set1(t[5], 128)
    addn([b[3]], [t[5]], *addw)
    set1(t[5], 128)
    addn([c[3]], [t[5]], *addw)
    for x in a:
        clear1(x)
    set1(more, 1)
    for i in range(3, -1, -1):
        copy1(more, run, t[5] + 1)
        clear1(more)
        go(run)
        out.append("[-")
        compare1(b[i], c[i], al, ag, w)
        set1(more, 1)
        go(al)
        out.append("[-")
        if op in ("lt", "le"):
            set1(a[0], 1)
        clear1(more)
        go(al)
        out.append("]")
        go(ag)
        out.append("[-")
        if op in ("gt", "ge"):
            set1(a[0], 1)
        clear1(more)
        go(ag)
        out.append("]")
        go(run)
        out.append("]")
    if op in ("eq", "le", "ge"):
        move1(more, a[0])
    else:
        clear1(more)
    go(a[0])


def abs4(nr, vx):
    a = cells(mem[nr]["at"])
    t = mem[nr]["tmp"]
    b = cells(t[0])
    w = cells(t[2]) + cells(t[3])
    load(vx, b, t[5])
    extract_sign(b[3], t[1], w)
    go(t[1])
    out.append("[-")
    negn(b, w[2], w[3], w[4], w[5])
    go(t[1])
    out.append("]")
    for i in range(4):
        clear1(a[i])
        move1(b[i], a[i])
    go(a[0])


def mul4(nr, vx, vy):
    a = cells(mem[nr]["at"])
    t = mem[nr]["tmp"]
    b = cells(t[0])
    c = cells(t[1])
    prod = cells(t[2]) + cells(t[3])
    w = cells(t[4]) + cells(t[5])
    load(vx, b, w[7])
    load(vy, c, w[7])
    extract_sign(b[3], a[0], w)
    extract_sign(c[3], a[1], w)
    clear1(a[2])
    go(a[0])
    out.append("[-")
    bool_flip(a[2], w[6])
    negn(b, w[2], w[3], w[4], w[5])
    go(a[0])
    out.append("]")
    go(a[1])
    out.append("[-")
    bool_flip(a[2], w[6])
    negn(c, w[2], w[3], w[4], w[5])
    go(a[1])
    out.append("]")
    for p in prod:
        clear1(p)
    for i in range(4):
        for j in range(4):
            copy1(c[j], w[0], w[1])
            go(w[0])
            out.append("[-")
            copy1(b[i], w[2], w[3])
            go(w[2])
            out.append("[-")
            incn(prod[i + j :], w[4], w[5], w[6], w[7])
            go(w[2])
            out.append("]")
            go(w[0])
            out.append("]")
    copy1(a[2], w[0], w[1])
    for i in range(4):
        copy1(prod[i + 2], a[i], w[1])
    go(w[0])
    out.append("[-")
    negn(a, w[2], w[3], w[4], w[5])
    go(w[0])
    out.append("]")
    go(a[0])


def div4(nr, vx, vy):
    a = cells(mem[nr]["at"])
    t = mem[nr]["tmp"]
    b = cells(t[0])
    c = cells(t[1])
    rem = cells(t[2]) + [t[3]]
    sg, sx, sy = t[3] + 1, t[3] + 2, t[3] + 3
    w = cells(t[4]) + cells(t[5])
    load(vx, b, w[7])
    load(vy, c, w[7])
    extract_sign(b[3], sx, w)
    extract_sign(c[3], sy, w)
    clear1(sg)
    go(sx)
    out.append("[-")
    bool_flip(sg, w[6])
    negn(b, w[2], w[3], w[4], w[5])
    go(sx)
    out.append("]")
    go(sy)
    out.append("[-")
    bool_flip(sg, w[6])
    negn(c, w[2], w[3], w[4], w[5])
    go(sy)
    out.append("]")
    for p in rem + a:
        clear1(p)
    nums = [b[3], b[2], b[1], b[0], None, None]
    sub = [t[3] + 2, t[3] + 3, w[0], w[1], w[2]]
    z, f, ca, cb, more = w[3], w[4], w[5], w[6], w[7]
    for digit in nums:
        clear1(rem[4])
        for i in range(4, 0, -1):
            move1(rem[i - 1], rem[i])
        clear1(rem[0])
        if digit is not None:
            copy1(digit, rem[0], f)
        clear1(a[3])
        for i in range(3, 0, -1):
            move1(a[i - 1], a[i])
        clear1(a[0])
        set1(more, 1)
        go(more)
        out.append("[-")
        copyn(c, sub[:4], 4, f)
        clear1(sub[4])
        negn(sub, z, f, ca, cb)
        carry = addn(rem, sub, z, f, ca, cb)
        set1(z, 1)
        go(carry)
        out.append("[-")
        go(a[0])
        out.append("+")
        clear1(z)
        go(more)
        out.append("+")
        go(carry)
        out.append("]")
        go(z)
        out.append("[-")
        copyn(c, sub[:4], 4, f)
        clear1(sub[4])
        addn(rem, sub, f, ca, cb, more)
        clear1(more)
        go(z)
        out.append("]")
        go(more)
        out.append("]")
    go(sg)
    out.append("[-")
    negn(a, w[2], w[3], w[4], w[5])
    go(sg)
    out.append("]")
    go(a[0])


def clear4(nx):
    a = mem[nx]["at"]
    for i in range(4):
        clear1(a + i)


def set4(nx, v):
    a = mem[nx]["at"]
    for i, b in enumerate(number(v)):
        go(a + i)
        out.append("[-]" + "+" * b)


def copy4(nr, nx):
    if nr == nx:
        return
    a = cells(mem[nr]["at"])
    b = cells(mem[nx]["at"])
    for i in range(4):
        copy1(b[i], a[i], mem[nr]["tmp"][0])


def int4(nr, nx):
    a = cells(mem[nr]["at"])
    t = mem[nr]["tmp"]
    b = cells(t[0])
    w = cells(t[2]) + cells(t[3])
    load(nx, b, t[5])
    extract_sign(b[3], t[1], w)
    copy1(t[1], t[1] + 1, t[1] + 2)
    go(t[1])
    out.append("[-")
    negn(b, w[0], w[1], w[2], w[3])
    go(t[1])
    out.append("]")
    clear1(b[0])
    clear1(b[1])
    go(t[1] + 1)
    out.append("[-")
    negn(b, w[0], w[1], w[2], w[3])
    go(t[1] + 1)
    out.append("]")
    for i in range(4):
        clear1(a[i])
        move1(b[i], a[i])


def subdigit(an, v, ad, ac, al, ae, ab, aw, az):
    def again():
        setn(ac, [v, 0])
        lessn(an, ac, al, ae, ab, aw)
        bool_flip(al, az[0])

    again()
    go(al)
    out.append("[")
    negn(ac, az[0], az[1], az[2], az[3])
    addn(an, ac, az[0], az[1], az[2], az[3])
    go(ad)
    out.append("+")
    again()
    go(al)
    out.append("]")


def digit1(ad, ac, at):
    copy1(ad, ac, at)
    go(ac)
    out.append("+" * 48 + ".")


def printnum(nx):
    t = mem[nx]["tmp"]
    an = cells(t[0], 2)
    ah, at = t[0] + 2, t[0] + 3
    ac = cells(t[1], 2)
    al, ae = t[1] + 2, t[1] + 3
    ab = cells(t[2], 2)
    aw = cells(t[3]) + cells(t[4])
    az = cells(t[5])
    copy1(mem[nx]["at"] + 2, an[0], az[0])
    copy1(mem[nx]["at"] + 3, an[1], az[0])
    clear1(ah)
    clear1(at)
    subdigit(an, 100, ah, ac, al, ae, ab, aw, az)
    subdigit(an, 10, at, ac, al, ae, ab, aw, az)
    clear1(ab[0])
    copy1(ah, ac[0], ac[1])
    truthiness([ac[0]], al)
    go(al)
    out.append("[-")
    digit1(ah, ac[0], ac[1])
    set1(ab[0], 1)
    go(al)
    out.append("]")
    copy1(ab[0], ac[0], ac[1])
    copy1(at, ac[1], az[0])
    truthiness(ac, al)
    go(al)
    out.append("[-")
    digit1(at, ac[0], ac[1])
    go(al)
    out.append("]")
    digit1(an[0], ac[0], ac[1])


def text1(s):
    for c in s:
        set1(printcell, ord(c))
        go(printcell)
        out.append(".")


funcs = {}
stack = []

for w in lines:
    if w[0] == "var":
        continue
    if w[0] == "func":
        funcs[w[1]] = []
        stack = [funcs[w[1]]]
    elif w[0] in ["if", "while"]:
        node = [w[0], w[1], [], None]
        stack[-1].append(node)
        stack.append(node[2])
    elif w[0] == "else":
        stack.pop()
        node = stack[-1][-1]
        node[3] = []
        stack.append(node[3])
    elif w[0] == "end":
        stack.pop()
    else:
        stack[-1].append(w)


def depth_of(body, depth=0):
    high = depth
    for w in body:
        if w[0] == "call":
            high = max(high, depth_of(funcs[w[1]], depth))
        if w[0] in ["if", "while"]:
            high = max(high, depth_of(w[2], depth + 1))
            if w[3] is not None:
                high = max(high, depth_of(w[3], depth + 1))
    return high


flow = p
p += depth_of(funcs["main"]) * 7
printcell = p
p += 1


def condition(nx, depth):
    a = flow + depth * 7
    copyn(cells(mem[nx]["at"]), cells(a), 4, a + 4)
    truthiness(cells(a), a + 5)
    return a + 5, a + 6


def emit(w, depth):
    if w[0] == "at":
        go(mem[w[1]]["at"] + (int(w[2]) if len(w) > 2 else 0))
    if w[0] == "zero":
        clear4(w[1])
    if w[0] == "set":
        set4(w[1], w[2])
    if w[0] == "copy":
        copy4(w[1], w[2])
    if w[0] == "add":
        add4(w[1], w[2], w[3])
    if w[0] == "sub":
        add4(w[1], w[2], w[3], True)
    if w[0] == "neg":
        neg4(w[1], w[2])
    if w[0] == "not":
        bool4(w[1], w[2])
    if w[0] == "and":
        bool4(w[1], w[2], w[3], True)
    if w[0] == "or":
        bool4(w[1], w[2], w[3])
    if w[0] == "mul":
        mul4(w[1], w[2], w[3])
    if w[0] == "div":
        div4(w[1], w[2], w[3])
    if w[0] in ["le", "lt", "ge", "gt", "eq"]:
        compare4(w[1], w[2], w[3], w[0])
    if w[0] == "abs":
        abs4(w[1], w[2])
    if w[0] == "sqrt":
        sqrt4(w[1], w[2])
    if w[0] == "int":
        int4(w[1], w[2])
    if w[0] == "text":
        text1(" ".join(w[1:]) + "\n")
    if w[0] in ["print2", "print3"]:
        for i, n in enumerate(w[1:]):
            if i:
                text1(" ")
            printnum(n)
        text1("\n")
    if w[0] == "call":
        emit_body(funcs[w[1]], depth)
    if w[0] == "if":
        af, ae = condition(w[1], depth)
        if w[3] is not None:
            set1(ae, 1)
        go(af)
        out.append("[-")
        if w[3] is not None:
            clear1(ae)
        emit_body(w[2], depth + 1)
        go(af)
        out.append("]")
        if w[3] is not None:
            go(ae)
            out.append("[-")
            emit_body(w[3], depth + 1)
            go(ae)
            out.append("]")
    if w[0] == "while":
        af, ae = condition(w[1], depth)
        go(af)
        out.append("[")
        emit_body(w[2], depth + 1)
        condition(w[1], depth)
        go(af)
        out.append("]")


def emit_body(body, depth):
    for w in body:
        emit(w, depth)


emit_body(funcs["main"], 0)

open("ray.bf", "w").write("".join(out))

with open("ray.map", "w") as f:
    print("cells", p, file=f)
    for n in mem:
        print(n, mem[n], file=f)

print(len(mem), "vars", p, "cells", len("".join(out)), "characters")
