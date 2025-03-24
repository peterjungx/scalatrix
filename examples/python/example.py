import scalatrix as sx

a1 = sx.Vector2d(0,0)
a2 = sx.Vector2d(2,5)
a3 = sx.Vector2d(1,3)

b1 = sx.Vector2d(0,0.2)
b2 = sx.Vector2d(1,0.2)
b3 = sx.Vector2d(0.585,0.3)

A = sx.affineFromThreeDots(a1,a2,a3,b1,b2,b3)
scale = sx.Scale.fromAffine(A, 265.0, 13, 0)
nodes = scale.getNodes()

for n in nodes:
    print(f"{n.natural_coord.x} {n.natural_coord.y} {n.tuning_coord.x:.3f} {n.tuning_coord.y:.3f} {n.pitch:.2f}")

