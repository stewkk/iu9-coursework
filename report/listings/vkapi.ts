const API_URL_PREFIX = "https://api.vk.com/method/";
const API_VERSION = "5.154";
const API_URL_SUFFIX = `&v=${API_VERSION}`;

type FriendsRequestParam = {
  key: string;
  value: string;
};

export class VkAPI {
  token: string;
  constructor(token: string) {
    this.token = token;
  }

  public getFriends(
    id: string,
    params: FriendsRequestParam[],
    callback: Function
  ) {
    let url = API_URL_PREFIX + `friends.get?user_id=${id}`;

    params.forEach(({ key, value }) => {
      url += `&${key}=${value}`;
    });

    this.makeApiRequest(url, "getFriends", callback);
  }

  public getMutual(sourceUid: string, users: User[], callback: Function) {
    let url =
      API_URL_PREFIX +
      `friends.getMutual?source_uid=${sourceUid}&target_uids=${users.reduce(
        (acc, user) => {
          return acc + user.id + ",";
        },
        ""
      )}`;
    this.makeApiRequest(url, "getMutual", callback);
  }
}
