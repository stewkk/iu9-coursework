export class Vertex {
  private _fullName: string;
  private _photoSrc: string;
  private _id: string;
  private _x: number;
  private _y: number;
  private _idx: number;

  public get fullName(): string {
    return this._fullName;
  }
  public get photoSrc(): string {
    return this._photoSrc;
  }
  public get id(): string {
    return this._id;
  }
  public get x(): number {
    return this._x;
  }
  public set x(value: number) {
    this._x = value;
  }
  public get y(): number {
    return this._y;
  }
  public set y(value: number) {
    this._y = value;
  }

  public get idx(): number {
    return this._idx;
  }

  public vector(): Vector {
    return new Vector(this._x, this._y);
  }

  constructor(user: User, idx: number) {
    this._fullName = user.first_name + " " + user.last_name;
    this._photoSrc = user.photo_100;
    this._id = user.id;
    this._x = Math.random() * 1000 + 1;
    this._y = Math.random() * 1000 + 1;
    this._idx = idx;
  }

}
