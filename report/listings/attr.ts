export const findAttractive = (lhs: Vector, rhs: Vector, k: number): Vector => {
  const delta = sub(rhs, lhs);
  const dist = delta.dist();

  const force = (dist * dist) / k;
  return mul(delta, force / dist);
};

