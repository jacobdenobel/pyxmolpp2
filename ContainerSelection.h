#pragma once

#include "Observer.h"


namespace selection {

template<typename T>
class Container;

template<typename T>
class Selection;


template<typename T>
struct ElementWithFlags{
  static_assert(!std::is_const_v<T>);
  static_assert(!std::is_reference_v<T>);
  static_assert(!std::is_pointer_v<T>);

  using container_type = Container<T>;

  ElementWithFlags(T&& value): value(std::move(value)),is_deleted(false){
  }
  ElementWithFlags(const T& value): value(std::move(value)),is_deleted(false){
  }
  T value;
  bool is_deleted = false;

private:
  friend class Selection<T>;
  const container_type* parent() const noexcept;
  container_type* parent() noexcept;
};


template<typename T>
struct SelectionTraits{
  static_assert(!std::is_reference_v<T>);
  static_assert(!std::is_pointer_v<T>);
  using value_type = std::remove_const_t<T>;
  using container_type = std::conditional_t<std::is_const_v<T>,const Container<value_type>,Container<value_type>>;
  using element_type = std::conditional_t<std::is_const_v<T>,
                                          const ElementWithFlags<value_type>,
                                          ElementWithFlags<value_type>>;
};





template<typename T>
class SelectionRange {
public:
  SelectionRange(const SelectionRange& other) noexcept = default;
  SelectionRange(SelectionRange&& other) noexcept = default;
  SelectionRange& operator=(const SelectionRange& other) noexcept  = default;
  SelectionRange& operator=(SelectionRange&& other) noexcept  = default;

  T& operator*() const;
  T* operator->() const;
  template<typename Sentinel>
  bool operator!=(const Sentinel&) const;

  SelectionRange& operator++();
  SelectionRange& operator--();

  SelectionRange operator-(int n);
  SelectionRange operator+(int n);
  SelectionRange& operator+=(int n);
  SelectionRange& operator-=(int n);

private:
  explicit SelectionRange(const Selection<T>& selection, int pos, int end, int step);
  friend class Selection<T>;
  const Selection<T>* selection;
  int pos;
  int end;
  int step;
};

template<typename T>
class Selection : public ObserverableBy<typename SelectionTraits<T>::container_type> {
public:
  static_assert(!std::is_reference_v<T>);
  static_assert(!std::is_pointer_v<T>);
  using value_type = typename SelectionTraits<T>::value_type;
  using container_type = typename SelectionTraits<T>::container_type;
  using element_type = typename SelectionTraits<T>::element_type;

  Selection();
  ~Selection();
  Selection(const Selection& rhs);
  Selection(Selection&& rhs) noexcept;
  Selection& operator=(const Selection& rhs);
  Selection& operator=(Selection&& rhs) noexcept;

  bool operator==(const Selection& rhs) const;
  bool operator!=(const Selection& rhs) const;

  Selection& operator+=(const Selection& rhs);
  Selection& operator-=(const Selection& rhs);

  size_t count(T&) const;

  bool empty() const;
  size_t size() const;


  SelectionRange<T> begin();
  SelectionRange<T> end();

private:
  explicit Selection(container_type& container);

  void on_container_move(container_type& from, container_type& to);
  void on_container_delete(container_type& half_dead);
  void on_container_elements_move(element_type* old_begin, element_type* old_end, ptrdiff_t shift);

  friend class SelectionRange<T>;
  friend class Container<value_type>;

  enum class SelectionState{
    OK,
    HAS_DANGLING_REFERENCES
  };

  std::vector<element_type*> elements;
  SelectionState state = SelectionState::OK;
};


template<typename T>
class Container : public ObserverableBy<Selection<T>>, public ObserverableBy<Selection<const T>>{
public:
  using value_type = typename SelectionTraits<T>::value_type;
  using element_type = typename SelectionTraits<T>::element_type;

  Container();
  ~Container();
  Container(Container&& rhs) noexcept ;
  Container(const Container& rhs);

  Container& operator=(Container&& rhs) noexcept ;
  Container& operator=(const Container& rhs);

  size_t size() const;
  bool empty() const;

  void insert(T&& value);

  size_t erase();
  size_t erase(Selection<T>& selection);

  Selection<T> all();
  Selection<const T> all() const;

  friend class Selection<T>;

protected:


  void on_selection_move(Selection<T>& from, Selection<T>& to);
  void on_selection_move(Selection<const T>& from, Selection<const T>& to) const;

  void on_selection_copy(Selection<T>& copy);
  void on_selection_copy(Selection<const T>& copy) const;

  void on_selection_delete(Selection<T>& half_dead);
  void on_selection_delete(Selection<const T>& half_dead) const;


  std::vector<element_type> elements;
  size_t deleted = 0;
};


template<typename  T>
SelectionRange<T>::SelectionRange(const Selection<T>& selection, int pos, int end, int step) :
    selection(&selection),
    pos(pos),
    end(end),
    step(step) {
}

template<typename  T>
T& SelectionRange<T>::operator*() const {
  assert(!this->selection->elements[pos]->is_deleted);
  return this->selection->elements[pos]->value;
}
template<typename  T>
T* SelectionRange<T>::operator->() const {
  assert(!this->selection->elements[pos].is_deleted);
  return this->selection->elements[pos].value;
}

template<typename T>
template<typename Sentinel>
bool SelectionRange<T>::operator!=(const Sentinel&) const {
  if (step>0){
    return pos<end;
  }else{
    return pos>end;
  }
}


template<typename T>
SelectionRange<T>& SelectionRange<T>::operator++() {
  pos+=step;
  return *this;
}

template<typename T>
SelectionRange<T>& SelectionRange<T>::operator--() {
  pos-=step;
  return *this;
}

template<typename T>
SelectionRange<T>& SelectionRange<T>::operator+=(int n) {
  pos+=step*n;
  return *this;
}

template<typename T>
SelectionRange<T>& SelectionRange<T>::operator-=(int n) {
  pos-=step*n;
  return *this;
}

template<typename T>
SelectionRange<T> SelectionRange<T>::operator-(int n) {
  SelectionRange<T> result(*this);
  result-=n;
  return result;
}

template<typename T>
SelectionRange<T> SelectionRange<T>::operator+(int n) {
  SelectionRange<T> result(*this);
  result+=n;
  return result;
}

template<typename T>
SelectionRange<T> Selection<T>::begin(){
  assert(state==SelectionState::OK);
  return SelectionRange(*this,0,elements.size(),1);
}

template<typename T>
SelectionRange<T> Selection<T>::end(){
  assert(state==SelectionState::OK);
  return SelectionRange(*this,elements.size(),0,1);
}

template<typename T>
Selection<T>::Selection() {
  LOG_DEBUG_FUNCTION();
  state=SelectionState::OK;
}

template<typename T>
Selection<T>::~Selection() {
  LOG_DEBUG_FUNCTION();
  constexpr auto alive_only = ObserverableBy<container_type>::ApplyTo::ALIVE_ONLY;
  this-> template notify_all<alive_only>(&container_type::on_selection_delete,*this);
}

template<typename T>
Selection<T>::Selection(const Selection<T>& rhs): ObserverableBy<container_type>(rhs), state(rhs.state), elements(rhs.elements) {
  LOG_DEBUG_FUNCTION();
  notify_all(&container_type::on_selection_copy,*this);
}

template<typename T>
Selection<T>::Selection(Selection<T>&& rhs) noexcept : ObserverableBy<container_type>(std::move(rhs)), state(rhs.state), elements(std::move(rhs.elements)){
  LOG_DEBUG_FUNCTION();
  this->notify_all(&container_type::on_selection_move,rhs,*this);
  rhs.state=SelectionState::OK;
}
template<typename T>
Selection<T>& Selection<T>::operator=(Selection<T>&& rhs) noexcept {
  LOG_DEBUG_FUNCTION();
  ObserverableBy<container_type>::operator=(std::move(rhs));
  elements = std::move(rhs.elements);
  state = rhs.state;
  rhs.state=SelectionState::OK;
  ObserverableBy<container_type>::notify_all(&container_type::on_selection_move,rhs,*this);
}

template<typename T>
Selection<T>& Selection<T>::operator=(const Selection<T>& rhs){
  LOG_DEBUG_FUNCTION();
  ObserverableBy<container_type>::operator=(rhs);
  elements =rhs.elements;
  state = rhs.state;
  notify_all(&container_type::on_selection_copy,*this);
}


template<typename T>
bool Selection<T>::operator==(const Selection<T>& rhs) const{
  LOG_DEBUG_FUNCTION();
  return elements==rhs.elements;
}

template<typename T>
bool Selection<T>::operator!=(const Selection<T>& rhs) const{
  LOG_DEBUG_FUNCTION();
  return elements!=rhs.elements;
}

template<typename T>
size_t Selection<T>::count(T& value) const {
  LOG_DEBUG_FUNCTION();
  assert(state==SelectionState::OK);
  return std::find_if(elements.begin(),elements.end(),
      [&value](const element_type& el){
    return el.value == value;
  }
  )==elements.end()?1:0;
}

template<typename T>
bool Selection<T>::empty() const {
  LOG_DEBUG_FUNCTION();
  return elements.empty();
}


template<typename T>
size_t Selection<T>::size() const {
  LOG_DEBUG_FUNCTION();
  return elements.size();
}

template<typename T>
void Selection<T>::on_container_move(Selection<T>::container_type& from, Selection<T>::container_type& to) {
  LOG_DEBUG_FUNCTION();
  move_observer(from,to);
}
template<typename T>
void Selection<T>::on_container_elements_move(element_type* old_begin, element_type* old_end, ptrdiff_t shift) {
  LOG_DEBUG_FUNCTION();
  for (auto& ptr: elements){
    if (old_begin<=ptr && ptr<old_end){
      ptr+=shift;
    }
  }
}

template<typename T>
void Selection<T>::on_container_delete(Selection<T>::container_type& half_dead) {
  LOG_DEBUG_FUNCTION();
  this->mark_as_deleted(half_dead);
  state=SelectionState::HAS_DANGLING_REFERENCES;
}

template<typename T>
Selection<T>::Selection(Selection<T>::container_type& container) {
  LOG_DEBUG_FUNCTION();
  state = SelectionState::OK;
  this->add_observer(container);
  for (element_type& el: container.elements){
    this->elements.push_back(&el);
  }
}


template<typename T>
Container<T>::Container(){
  LOG_DEBUG_FUNCTION();
}

template<typename T>
Container<T>::~Container(){
  LOG_DEBUG_FUNCTION();
  ObserverableBy<Selection<T>>::notify_all(&Selection<T>::on_container_delete,*this);
  ObserverableBy<Selection<const T>>::notify_all(&Selection<const T>::on_container_delete,*this);
}

template<typename T>
Container<T>::Container(Container<T>&& rhs) noexcept : ObserverableBy<Selection<const T>>(std::move(rhs)), ObserverableBy<Selection<T>>(std::move(rhs)){
  LOG_DEBUG_FUNCTION();
  notify_all(&Selection<T>::on_container_move,rhs,*this);
}

template<typename T>
Container<T>::Container(const Container<T>& rhs){
  LOG_DEBUG_FUNCTION();
}

template<typename T>
Container<T>& Container<T>::operator=(Container<T>&& rhs) noexcept {
  LOG_DEBUG_FUNCTION();
  elements = std::move(rhs.elements);
  deleted = rhs.deleted;
  rhs.deleted = 0;
  ObserverableBy<Selection<T>>::operator=(std::move(rhs));
  ObserverableBy<Selection<T>>::notify_all(&Selection<T>::on_container_move,rhs,*this);
  ObserverableBy<Selection<const T>>::operator=(std::move(rhs));
  ObserverableBy<Selection<const T>>::notify_all(&Selection<const T>::on_container_move,rhs,*this);
  return *this;
}

template<typename T>
Container<T>& Container<T>::operator=(const Container<T>& rhs) {
  LOG_DEBUG_FUNCTION();
  elements = rhs.elements;
  deleted = rhs.deleted;
  return *this;
}


template<typename T>
void Container<T>::insert(T&& value) {
  LOG_DEBUG_FUNCTION();
  auto old_begin = elements.data();
  elements.reserve(elements.size()+1);
  auto new_pos= elements.data();

  ptrdiff_t shift = new_pos-old_begin;
  if (shift!=0){
    ObserverableBy<Selection<const T>>::notify_all(&Selection<const T>::on_container_elements_move,old_begin,old_begin+elements.size(),shift);
    ObserverableBy<Selection<T>>::notify_all(&Selection<T>::on_container_elements_move,old_begin,old_begin+elements.size(),shift);
  }
  elements.emplace_back(std::move(value));
}



template<typename T>
Selection<T> Container<T>::all(){
  LOG_DEBUG_FUNCTION();
  Selection<T> s(*this);
  ObserverableBy<Selection<T>>::add_observer(s);
  return std::move(s);
}

template<typename T>
Selection<const T> Container<T>::all() const{
  LOG_DEBUG_FUNCTION();
  Selection<const T> s(*this);
  ObserverableBy<Selection<const T>>::add_observer(s);
  return std::move(s);
}

template<typename T>
void Container<T>::on_selection_move(Selection<T>& from, Selection<T>& to){
  LOG_DEBUG_FUNCTION();
  ObserverableBy<Selection<T>>::move_observer(from,to);
}

template<typename T>
void Container<T>::on_selection_move(Selection<const T>& from, Selection<const T>& to) const{
  LOG_DEBUG_FUNCTION();
  ObserverableBy<Selection<const T>>::move_observer(from,to);
}


template<typename T>
void Container<T>::on_selection_delete(Selection<T>& half_dead){
  LOG_DEBUG_FUNCTION();
  ObserverableBy<Selection<T>>::remove_observer(half_dead);
}

template<typename T>
void Container<T>::on_selection_delete(Selection<const T>& half_dead) const{
  LOG_DEBUG_FUNCTION();
  ObserverableBy<Selection<const T>>::remove_observer(half_dead);
}


template<typename T>
void Container<T>::on_selection_copy(Selection<T>& copy){
  LOG_DEBUG_FUNCTION();
  ObserverableBy<Selection<T>>::add_observer(copy);
}

template<typename T>
void Container<T>::on_selection_copy(Selection<const T>& copy) const{
  LOG_DEBUG_FUNCTION();
  ObserverableBy<Selection<const T>>::add_observer(copy);
}




}
