export const sub = (lhs: Vector, rhs: Vector): Vector => {
  return new Vector(lhs.x - rhs.x, lhs.y - rhs.y);
};

export const sum = (lhs: Vector, rhs: Vector): Vector => {
  return new Vector(lhs.x + rhs.x, lhs.y + rhs.y);
};

export const mul = (vec: Vector, num: number): Vector => {
  return new Vector(vec.x * num, vec.y * num);
};

export const findDistance = (lhs: Vertex, rhs: Vertex): number => {
  return Math.sqrt(Math.pow(rhs.x - lhs.x, 2) + Math.pow(rhs.y - lhs.y, 2));
};

export const findVector = (lhs: Vertex, rhs: Vertex): Vector => {
  let result = new Vector(rhs.x - lhs.x, rhs.y - lhs.y);
  let dist = findDistance(lhs, rhs);

  result.x /= dist;
  result.y /= dist;

  return result;
};
