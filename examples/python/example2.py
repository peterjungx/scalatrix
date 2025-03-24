import scalatrix as sx

sx.AffineTransform.__repr__ = lambda self: f"M({self.a:.3f} {self.b:.3f} {self.c:.3f} {self.d:.3f})+({self.tx:.3f} {self.ty:.3f})"
sx.Vector2d.__repr__ = lambda self: f"V({self.x:.3f} {self.y:.3f})"

sx.AffineTransform.det = lambda self: self.a*self.d - self.b*self.c

a1 = sx.Vector2d(0,0)
a2 = sx.Vector2d(2,0)
a3 = sx.Vector2d(3,1)

A1 = sx.affineFromThreeDots(a1,a2,a3,a1,a3,a2)
A2 = sx.affineFromThreeDots(a1,a2,a3,a2,a1,a3)
A3 = sx.affineFromThreeDots(a1,a2,a3,a3,a2,a1)
A4 = sx.affineFromThreeDots(a1,a2,a3,a2,a3,a1)
A5 = sx.affineFromThreeDots(a1,a2,a3,a3,a1,a2)

print(A1, f"{A1.det():.2f}")
print(A2, f"{A2.det():.2f}")
print(A3, f"{A3.det():.2f}")
print(A4, f"{A4.det():.2f}")
print(A5, f"{A5.det():.2f}")


#c = A.apply(a1)
#print(f"{c.x:.3f} {c.y:.3f}")
#
#c = A.apply(a2)
#print(f"{c.x:.3f} {c.y:.3f}")
#
#c = A.apply(a3)
#print(f"{c.x:.3f} {c.y:.3f}")

def printCycle(A, c, n):
    for i in range(n):
        print(c, end=" -> ")
        c = A.apply(c)
    print(c)

v = sx.Vector2d(1,0)
printCycle(A1, v, 2)
printCycle(A2, v, 2)
printCycle(A3, v, 2)
printCycle(A4, v, 3)
printCycle(A5, v, 3)

