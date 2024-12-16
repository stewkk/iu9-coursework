const floydWarshall = (graph: Graph): number[][] => {
  const verticesCount = graph.vertexCount;

  let dist = new Array(verticesCount);

  for (let i = 0; i < verticesCount; ++i) {
    dist[i] = new Array(verticesCount);

    for (let j = 0; j < verticesCount; j++) {
      dist[i][j] = Number.MAX_SAFE_INTEGER / 2 - 1;
    }
  }

  for (const u in graph.vertices) {
    const i = graph.vertices[u].idx;

    dist[i][i] = 0;

    for (const v in graph.adjMatrix[u]) {
      const vertex = graph.vertices[graph.adjMatrix[u][v]];
      dist[i][vertex.idx] = 1;
      dist[vertex.idx][i] = 1;
    }
  }

  for (let k = 0; k < Object.keys(graph.vertices).length; k++) {
    for (let i = 0; i < Object.keys(graph.vertices).length; i++) {
      for (let j = 0; j < Object.keys(graph.vertices).length; j++) {
        dist[i][j] = Math.min(dist[i][j], dist[i][k] + dist[k][j]);
      }
    }
  }

  return dist;
};
