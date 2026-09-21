import re

s = open("ray_ssa.c").read()

def endbrace(p):
    n = 1
    while n:
        p += 1
        n += (s[p] == "{") - (s[p] == "}")
    return p


funcs = []
for fn in re.finditer(r"\b(?:void|int|double)\s+(\w+)\s*\((.*?)\)\s*\{", s, re.S):
    start = fn.start()
    end = endbrace(fn.end() - 1) + 1
    fn_name = fn.group(1)
    args = fn.group(2)
    body = fn.end()
    funcs.append((start, end, fn_name, args, body))

decl = re.compile(r"\b(?:int|double)\s+\**\s*(\w+)\s*(?:=[^;]*)?;")
found = []
outside = list(s)

for start, end, fn_name, args, body in funcs:
    outside[start:end] = " " * (end - start)
    for i, arg in enumerate(args.split(",")):
        m = re.search(r"\b(?:int|double)\s+\**\s*(\w+)\s*$", arg.strip())
        if m:
            found.append((start + i, fn_name + "_" + m.group(1)))
    for m in decl.finditer(s[body : end - 1]):
        found.append((body + m.start(), fn_name + "_" + m.group(1)))

for m in decl.finditer("".join(outside)):
    found.append((m.start(), m.group(1)))

names = [n for _, n in sorted(found)]

open("ray.dsl", "w").write("".join("var " + n + "\n" for n in names))

print(len(names), "vars")
