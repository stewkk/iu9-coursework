  private findVertexEnergy = (v: Vertex): number => {
    let xEnergy = 0.0;
    let yEnergy = 0.0;

    const vVec = Vector.fromVertex(v);
    for (const id in this.graph.vertices) {
      const u = this.graph.vertices[id];
      const uVec = Vector.fromVertex(u);
      if (u.idx == v.idx) continue;

      const delta = sub(vVec, uVec);
      const dist = delta.dist();

      const spring = this.springs[v.idx][u.idx];
      xEnergy += delta.x * spring.strength * (1.0 - spring.length / dist);
      yEnergy += delta.y * spring.strength * (1.0 - spring.length / dist);
    }

    return Math.sqrt(xEnergy * xEnergy + yEnergy * yEnergy);
  };
