import scalatrix as sx

#sx.AffineTransform.__repr__ = lambda self: f"M({self.a:.3f} {self.b:.3f} {self.c:.3f} {self.d:.3f})+({self.tx:.3f} {self.ty:.3f})"
#sx.Vector2d.__repr__ = lambda self: f"V({self.x:.3f} {self.y:.3f})"
#sx.AffineTransform.det = lambda self: self.a*self.d - self.b*self.c

mos = sx.MOS.fromG(4, 11, 0.585, 1.0, 1)
mos12tet = sx.MOS.fromG(3, 1, 7/12, 1.0, 1)
pitchset = sx.PitchSet.generateETPitchSet(12, 1.0)
primelist = sx.PrimeList.generateDefaultPrimeList(6)
primelist = primelist[0:2] + primelist[3:]

pitchset = sx.PitchSet.generateJIPitchSet(primelist, 240, 0, 1)



for p in pitchset:
    print(p.label, p.log2fr)

#mos.base_scale.temperToPitchSet(pitchset)

print(mos.a, mos.b, mos.equave, mos.generator, (7/12))

for n in mos.base_scale.getNodes():
    print(f"({n.natural_coord.x}, {n.natural_coord.y}) ({1200*n.tuning_coord.x:.3f}, {n.tuning_coord.y:.3f}) {n.pitch:.3f} ")

print("")
s = mos.generateScaleFromMOS(220*2**(7/12), 128, 60)
for n in s.getNodes()[58:75]:
    label_western = mos12tet.nodeLabelLetterWithOctaveNumber(mos12tet.mapFromMOS(mos, n.natural_coord), 5)
    print(f"({n.natural_coord.x}, {n.natural_coord.y}) ({1200 * n.tuning_coord.x:.3f}, {n.tuning_coord.y:.3f}) {n.pitch:.3f} {label_western}", end=" ")
    if n.isTempered:
        print(f"({n.temperedPitch.label})")
    else:
        print("")

print(220*2**(9/12))




