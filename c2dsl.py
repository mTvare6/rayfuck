import re

s = open("ray_ssa.c").read()
vars = re.findall(r"^\s*(?:double|int)\s+(\w+)\s*;", s, re.M)
out = ["var " + n for n in vars]
ops = {
    "+": "add",
    "-": "sub",
    "*": "mul",
    "/": "div",
    "<": "lt",
    ">": "gt",
    "<=": "le",
    ">=": "ge",
    "==": "eq",
    "!=": "ne",
    "&&": "and",
    "||": "or",
}
value = r"(?:\w+|-?(?:\d+(?:\.\d*)?|\.\d+)(?:e[+-]?\d+)?)"

for line in s.splitlines():
    line = line.strip()
    fn = re.fullmatch(r"(?:void|int)\s+(\w+)\(void\)\s*\{", line)
    if fn:
        out.append("func " + fn.group(1))
        continue
    if not line or line.startswith("#") or re.fullmatch(r"(?:double|int)\s+\w+;", line):
        continue
    if line == "} else {":
        out.append("else")
        continue
    if line == "}":
        out.append("end")
        continue
    body = re.fullmatch(r"(if|while)\s*\((\w+)\)\s*\{", line)
    if body:
        out.append(body.group(1) + " " + body.group(2))
        continue
    call = re.fullmatch(r"(\w+)\(\);", line)
    if call:
        out.append("call " + call.group(1))
        continue
    if line == 'printf("P3\\n");':
        out.append("text P3")
        continue
    if line.startswith("printf"):
        names = re.findall(r"\(int\)(\w+)", line)
        out.append(("print2" if len(names) == 2 else "print3") + " " + " ".join(names))
        if "\\n255\\n" in line:
            out.append("text 255")
        continue
    if line == "return 0;":
        continue
    assign = re.fullmatch(r"(\w+)\s*=\s*(.*);", line)
    if not assign:
        continue
    a, b = assign.groups()
    one = re.fullmatch(r"(sqrt|fabs)\((\w+)\)", b)
    if one:
        out.append(
            ("sqrt" if one.group(1) == "sqrt" else "abs") + " " + a + " " + one.group(2)
        )
        continue
    cast = re.fullmatch(r"\(int\)(\w+)", b)
    if cast:
        out.append("int " + a + " " + cast.group(1))
        continue
    if re.fullmatch(r"!\w+", b):
        out.append("not " + a + " " + b[1:])
        continue
    if re.fullmatch(r"-[A-Za-z]\w*", b):
        out.append("neg " + a + " " + b[1:])
        continue
    two = re.fullmatch(
        "(" + value + r")\s*(<=|>=|==|!=|&&|\|\||[+*/<>-])\s*(" + value + ")", b
    )
    if two:
        x, op, y = two.groups()
        out.append(ops[op] + " " + a + " " + x + " " + y)
        continue
    out.append(
        ("set " if re.fullmatch(value, b) and not b[0].isalpha() else "copy ")
        + a
        + " "
        + b
    )

open("ray.dsl", "w").write("\n".join(out) + "\n")
print(len(vars), "vars", len(out) - len(vars), "lines")
