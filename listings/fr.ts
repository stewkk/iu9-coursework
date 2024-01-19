export const FruchtermanReingold = ( graph: Graph, k: number, iterCount: number) => {
  graph = makeCircle(graph);
  let mp: { [_: string]: Vector } = {};
  let t = 10 * Math.sqrt(Object.keys(graph.vertices).length);
  for (let l = 0; l < iterCount; l++) {
    for (const u in graph.vertices) {
      mp[u] = new Vector(0, 0);
    }
    const set = new Set();
    for (const u in graph.vertices) {
      for (const v in graph.vertices) {
        if (u == v || set.has([u, v] || set.has([v, u]))) continue;

        const [force, dist] = findRepulsive(
          Vector.fromVertex(graph.vertices[v]),
          Vector.fromVertex(graph.vertices[u]),
          k,
        );
        if (dist > 2000) continue;
        mp[u] = sum(mp[u], force);
        mp[v] = sub(mp[v], force);
      }

      for (const j in graph.adjMatrix[u]) {
        const v = graph.adjMatrix[u][j];
        if (graph.vertices[v].idx > graph.vertices[u].idx) continue;
        const force = findAttractive(Vector.fromVertex(graph.vertices[v]), Vector.fromVertex(graph.vertices[u]), k);
        mp[u] = sub(mp[u], force);
        mp[v] = sum(mp[v], force);
      }
    }
    for (const u in graph.vertices) {
      const norm = mp[u].dist();
      if (norm < 1) continue;
      const tmp = Math.min(norm, t);
      const diff = mul(mp[u], tmp / norm);
      graph.vertices[u].x += diff.x;
      graph.vertices[u].y += diff.y;
    }
    t = t > 1.5 ? t * 0.85 : 1.5
  }
  return graph;
};
