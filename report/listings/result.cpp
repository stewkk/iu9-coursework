struct EmptyResult {};

template <typename T = EmptyResult> using Result = boost::outcome_v2::result<T, Error>;

template <typename T = EmptyResult> auto Ok(T&& v = {}) { return boost::outcome_v2::success(v); }

template <ErrorType ErrorType = ErrorType::kUnknown, typename... Args>
Error MakeError(const std::format_string<Args...> fmt, Args&&... args) {
  return Error{ErrorType, std::format(fmt, std::forward<Args>(args)...)};
}

template <ErrorType ErrorType = ErrorType::kUnknown, typename T, typename... Args>
Error WrapError(Result<T>&& result, const std::format_string<Args...> fmt, Args&&... args) {
  return result.assume_error().Wrap(ErrorType, std::format(fmt, std::forward<Args>(args)...));
}

std::string What(const Error& error);
