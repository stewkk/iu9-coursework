import { User } from "./user";

export type ApiResponse = {
  response: GetMutualResponse | GetFriendsResponse;
} & ErrorResponse;

export type ErrorResponse = {
  error: string;
};

export type GetFriendsResponse = {
  count: Number;
  items: User[];
};

export type GetMutualResponse = {
  common_count: Number;
  common_friends: Number;
  id: string;
}[];
