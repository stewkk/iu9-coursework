private findNextVertexPos = (v: Vertex): Vector => {
    let xxEnergy = 0.0,
      xyEnergy = 0.0,
      yxEnergy = 0.0,
      yyEnergy = 0.0;
    let xEnergy = 0.0,
      yEnergy = 0.0;

    const vVec = Vector.fromVertex(v);
    for (const u in this.graph.vertices) {
      const uVertex = this.graph.vertices[u];
      const uVec = Vector.fromVertex(uVertex);
      if (uVertex.idx == v.idx) continue;

      const delta = sub(vVec, uVec);
      const dist = delta.dist();

      const spring = this.springs[v.idx][uVertex.idx];

      xEnergy += delta.x * spring.strength * (1.0 - spring.length / dist);
      yEnergy += delta.y * spring.strength * (1.0 - spring.length / dist);
      xyEnergy +=
        (spring.strength * spring.length * delta.x * delta.y) /
        Math.pow(dist, 3);
      xxEnergy +=
        spring.strength *
        (1.0 - (spring.length * delta.y * delta.y) / Math.pow(dist, 3));
      yyEnergy +=
        spring.strength *
        (1.0 - (spring.length * delta.x * delta.x) / Math.pow(dist, 3));
    }

    yxEnergy = xyEnergy;
    const denominator = xxEnergy * yyEnergy - xyEnergy * yxEnergy;
    vVec.x += (xyEnergy * yEnergy - yyEnergy * xEnergy) / denominator;
    vVec.y += (xyEnergy * xEnergy - xxEnergy * yEnergy) / denominator;

    return vVec;
  };
