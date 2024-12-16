

export class Graph {
  private _vertices: { [_: string]: Vertex } = {};
  private _adjMatrix: { [_: string]: string[] } = {};
  private _vertexCount: number;

  public get vertexCount(): number {
    return this._vertexCount;
  }

  public set vertexCount(value: number) {
    this._vertexCount = value;
  }

  public get adjMatrix(): { [_: string]: string[] } {
    return this._adjMatrix;
  }

  public set adjMatrix(value: { [_: string]: string[] }) {
    this._adjMatrix = value;
  }

  public get vertices(): { [_: string]: Vertex } {
    return this._vertices;
  }
  public set vertices(value: { [_: string]: Vertex }) {
    this._vertices = value;
  }

  constructor(adj: { [_: string]: string[] }, users: { [_: string]: User }) {
    this.adjMatrix = adj;
    this._vertexCount = Object.keys(users).length;

    Object.keys(users).forEach((key, idx) => {
      if (!this.adjMatrix[key]) {
        this.adjMatrix[key] = [];
      }
      this.vertices[key] = new Vertex(users[key], idx);
    });
  }
}
