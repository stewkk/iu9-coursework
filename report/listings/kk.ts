public run = (width: number, height: number): Graph => {
    let steady_energy_count = 0;
    let [maxEnergy, id] = this.findMaxVertexEnergy();

    while (
      maxEnergy > this.eps &&
      steady_energy_count < MAX_STEADY_ENERGY_ITERS_COUNT
    ) {

      let vertex_count = 0;
      do {
        let newPos = this.findNextVertexPos(this.graph.vertices[id]);
        this.graph.vertices[id].x = newPos.x;
        this.graph.vertices[id].y = newPos.y;
        vertex_count++;
      } while (
        this.findVertexEnergy(this.graph.vertices[id]) > this.eps &&
        vertex_count < MAX_STEADY_ENERGY_ITERS_COUNT
      );

      let prevMax = maxEnergy;
      [maxEnergy, id] = this.findMaxVertexEnergy();
      if (Math.abs(maxEnergy - prevMax) < 1e-20) {
        steady_energy_count++;
      } else {
        steady_energy_count = 0;
      }
    }

    return centerAndScale(this.graph, width, height);
};
