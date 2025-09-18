Result<PersistentStorage> MergeData(PersistentStorageCollection& collection) {
  const auto& storages = collection.GetCollection();
  assert(storages.size() > 0);
  using StorageIterator = decltype(storages.front().begin());
  using IteratorPair = std::tuple<StorageIterator, StorageIterator, size_t>;
  std::vector<IteratorPair> iterators;
  iterators.reserve(storages.size());
  for (size_t i = 0; i < storages.size(); i++) {
    const auto& storage = storages[i];
    iterators.emplace_back(storage.begin(), storage.end(), i);
  }
  std::vector<StorageEntry> data;
  auto comparator = [](const IteratorPair& lhs, const IteratorPair& rhs) {
    auto lhs_opt = *(std::get<0>(lhs));
    auto rhs_opt = *(std::get<0>(rhs));
    if (lhs_opt.has_failure()) return false;
    if (rhs_opt.has_failure()) return false;
    auto lhs_value = std::move(lhs_opt).assume_value();
    auto rhs_value = std::move(rhs_opt).assume_value();
    if (lhs_value.key == rhs_value.key) return std::get<2>(lhs) < std::get<2>(rhs);
    return lhs_value.key > rhs_value.key;
  };
  std::priority_queue<IteratorPair, decltype(iterators), decltype(comparator)> queue(
      iterators.begin(), iterators.end(), comparator);
  while (!queue.empty()) {
    auto [cur, end, index] = queue.top();
    queue.pop();
    auto data_opt = *cur;
    if (data_opt.has_failure())
      return result::WrapError(std::move(data_opt),
                               "failed to merge data from persistent storages");
    auto value = std::move(data_opt).assume_value();
    if (data.empty() || data.back().key != value.key) data.push_back(std::move(value));
    ++cur;
    if (cur != end) queue.push({cur, end, index});
  }
  return NewPersistentStorage(std::move(data));
}
