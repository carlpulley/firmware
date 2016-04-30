#pragma once

template <class T>
class optional {
  T *value;

  public:
    optional() {
      this->value = NULL;
    }

    optional(T value) {
      this->value = &value;
    }

    bool isDefined() {
      return value == NULL;
    }

    T get() {
      return *value;
    }
};
