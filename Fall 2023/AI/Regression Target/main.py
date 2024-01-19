import math
import numpy as np


def H(p):
    return -p * math.log2(p) - (1 - p) * math.log2(1 - p)


def regresion_IG(a: [], b: []):
    c = []
    c.extend(a)
    c.extend(b)
    base_var = np.var(c)
    IG = len(a) / len(c) * (base_var - np.var(a)) + len(b) / len(c) * (
        base_var - np.var(b)
    )
    print("IG:", IG)


if __name__ == "__main__":
    regresion_IG([8, 6, 9, 2], [5, 1, 3, 4])
    regresion_IG([8, 9, 1, 4], [6, 5, 2, 3])
    regresion_IG([8, 6, 9, 1, 3, 4], [5, 2])
