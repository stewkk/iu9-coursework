const draw = (graph: Graph, cy: cytoscape.Core) => {
  Object.keys(graph.vertices).forEach((id) => {
    const vertex = graph.vertices[id];
    cy.add({
      group: "nodes",
      data: { id: id, fullName: vertex.fullName, avatar: vertex.photoSrc },
      position: {
        x: graph.vertices[id].x,
        y: graph.vertices[id].y,
      },
    });
  });

  Object.keys(graph.adjMatrix).forEach((lhs) => {
    graph.adjMatrix[lhs].forEach((rhs) => {
      cy.add({
        group: "edges",
        data: { id: lhs + rhs, source: lhs, target: rhs },
      });
    });
  });
};
