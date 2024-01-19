  private findMaxVertexEnergy = (): [number, string] => {
    let max = -1.0;
    let maxId = "";
    for (let id in this.graph.vertices) {
      const energy = this.findVertexEnergy(this.graph.vertices[id]);
      if (energy > max) {
        maxId = id;
        max = energy;
      }
    }
    return [max, maxId];
  };
