const findRepulsive = (
  lhs: Vector,
  rhs: Vector,
  k: number,
): [Vector, number] => {
  const delta = sub(rhs, lhs);
  const dist = delta.dist();
  const force = (k * k) / dist;
  return [mul(delta, force / dist), dist];
};
