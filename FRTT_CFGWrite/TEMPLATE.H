#ifndef BAMI_TEMPLATES
#define BAMI_TEMPLATES

#include "macro.h"
#include "myStream.h"






// MyArray
////////////////////////////////////////////////////////////////////////////////
template  <class T > class  MyArray {
	T **info;
	 long size;
	 long total;
	 long delta;
	 bool autoDelete;
 public:
  T* Clone(){ return new T(); };
	MyArray(long _Delta = 20,bool _autoDelete = true){
			info=0; size =0; delta =_Delta; total = 0;autoDelete = _autoDelete;
			 //Debug("\nMyArray<%s>::MyArray",typeid(T).name());

	};
	~MyArray(){
		
		Clear();
	 };

	void SetDelta(long _delta){
		if(_delta>0) delta=_delta;
		else delta =1;
	};

	void DeleteOrder(bool _autoDelete){
		autoDelete = _autoDelete;
	};

	bool IsAutoDelete(){
		return autoDelete;
	};

	T* Get(long idx) const;
	void Erase(long idx);
	long Add (T *v);
	long Insert(T *v, long Befor);
	void Clear(void);
	long Where(T *v);
	void Destroy(T *v){ long id=Where(v); if(size>0  && id>=0)Erase(id); };
	void Destroy(long id){ Erase(id); };
	long Count() const { return size;};
	T* operator [] (long idx) const { return Get(idx);};
	T* operator [] (int idx) const { return Get(idx);};
	void Set(long idx, T* v);

 
};



template <class T> long MyArray<T>::Where(T *v){
		for(int i =0;i<size;i++)
			if (((void*)(info[i])) ==((void*) v))
				return i;
		return -1;
};

template <class T> 	void MyArray<T>::Erase(long cur){
	if ( cur >= 0 && cur < size){
		if(autoDelete){
				if( info ){
					ifdel (info[cur]);
				}
		//}else {
		//	Debug("\nInfo not deleted at %d size=%d",cur,sizeof(T));
		}
		info[cur]=0;
	int i;
	for (i=cur+1;i<size;i++)
		info[i-1]=info[i];
	size --;
	}
};

template <class T> 	void MyArray<T>::Set(long cur,T* v){
	if ( cur >= 0 && cur < size){
		info[cur] = v;
	}
};

template <class T>	T* MyArray<T>::Get(long idx)const {
		if (idx>=0 && idx <size)
			return info[idx];
		else{
				 //Debug("\nDanger 0 returned!!!  MyArray<%s>::Get idx=%ld (size=%d)",typeid(T).name(),idx,size);
			return 0;
			}
};

template <class T> long MyArray<T>::Add (T* v){
		 //Debug("\nMyArray<%s>::Add %p",typeid(T).name(),v);
		if (total==size){
			T **info2;
			info2 = new T* [total+delta];
				memset(info2,0,sizeof(T*) *(total+delta));
			for (int i = 0; i<size;i++)
				info2[i] = info[i];
			if(info) delete [] info;
				info = info2;
			 total+=delta;
		}
	    info[size++] = v;
		return size;
};

template <class T>  void MyArray<T>::Clear(void){

		for (int i = 0; i<size; i++){
				 //Debug("\nMyArray<%s>::Clear %p",typeid(T).name(),info[i]);
			if(autoDelete){
					if(info && info[i] ) {
						delete info[i];
					}
			//}else{
			//	Debug("\nInfo not deleted at %d size=%d",i,sizeof(T));
			}
		}
		if (info) delete [] info;
		total=size=0;
		info = 0;
};


template <class T>  long MyArray<T>::Insert(T *v, long Befor){
	Add(v);

	if(Befor >= 0 && Befor < size-1){
		for(long i=size-2;i>=Befor;i--)
			info[i+1]=info[i];
		info[Befor]=v;
	}
	return Befor;
};





// MyStack
////////////////////////////////////////////////////////////////////////////////
template  <class T > class  MyStack {
	T **info;
	long size;
	long total;
	long delta;
 public:
	MyStack(long _Delta = 20){
			info=0; size =0; delta =_Delta; total = 0;
	};

	~MyStack(){
			Clear();
	 };

	void SetDelta(long _delta){
		if(_delta>0) delta=_delta;
		else delta =1;
	};

	void Clear(void);

	T* Get(long Index);

	T* pop(){
		if (size>0)
			return info[--size];
		else
			return 0;
	};

	T* peek(){
		if (size>0)
			return info[size-1];
		else
			return 0;
	};


	void push(T*v){
		Add(v);
	};

	long Count() const { return size;};
 private:
	long Add (T *v);

};


template <class T>	T* MyStack<T>::Get(long idx){
		if (idx>=0 && idx <size)
			return info[idx];
		else{
				 //Debug("\nDanger 0 returned!!!  MyArray<%s>::Get idx=%ld (size=%d)",typeid(T).name(),idx,size);
			return 0;
			}
};


template <class T> long MyStack<T>::Add (T* v){
		if (total==size){
			T **info2;
			info2 = new T* [total+delta];
				memset(info2,0,sizeof(T*) *(total+delta));
			for (int i = 0; i<size;i++)
				info2[i] = info[i];
			if(info) delete [] info;
				info = info2;
			 total+=delta;
		}
	 info[size++] = v;
	 return size;
};

template <class T>  void MyStack<T>::Clear(void){
		if (info) delete [] info;
		total=size=0;
		info = 0;
};



// MyHashList
////////////////////////////////////////////////////////////////

template <class T> class HashItem {
		long HashValue;
		T* ptr;
public:

		const T * GetPtr(){ return ptr; }
		unsigned long GetHash(){ return HashValue; }

		HashItem(T * _ptr,unsigned long Value){
				HashValue =Value;
				ptr=_ptr;
				next=0;
				prev=0;
		};

		HashItem<T> *next;
		HashItem<T> *prev;
};


template <class T> class MyHashList{
		HashItem<T> *_Cur;
		HashItem<T> *_First;
		HashItem<T> *_Last;
		unsigned long HashValue;
		bool AutoDelete;

		virtual unsigned long MakeHash(char *  Name){
				unsigned long hv=0;
				for(short i=0;i<3 ;i++){
						hv<<8;
						if(i<strlen(Name))
						hv+=(unsigned char) Name[i];
				}
				return hv;
		}

public:
		MyHashList(bool _AutoDelete=false){
								_First=_Last=_Cur=0;
								HashValue=0;
								AutoDelete=_AutoDelete;
		};

		void DeleteOrder( bool _AutoDelete){AutoDelete=_AutoDelete;};

		void Del(T * ptr, char* Name){
				HashItem<T> *zz=FindFirst(Name);
				unsigned long hv=MakeHash(Name);
				while(zz){
					if(zz->GetPtr()==ptr &&  hv==zz->GetHash()){
						if(!zz->prev)
							_First=zz->next;
						if(!zz->next)
							_Last=zz->prev;

						if(zz->next && zz->prev){
							zz->next->prev=zz->prev;
							zz->prev->next=zz->next;
						}
						break;
					}
					zz=zz->next;

				}
				if(_Cur==zz) _Cur=_First;
				if(AutoDelete){
				 ifdel (zz->ptr);
				}
				delete zz;
		};

		HashItem<T> * ReHash(T * ptr, char* OldName, char * Name){
								bool aa =AutoDelete;
								AutoDelete=false;
								Del(ptr,OldName);
								AutoDelete=aa;
								return Add(ptr,Name);
		};

		HashItem<T> * Add(T * ptr, char* Name){
				HashItem<T> *item = new HashItem<T>(ptr,MakeHash(Name));

				if(!_First && !_Last){
						_First=item;
						_Last=item;
						return item;
				}

				// проверяем края
				if(_First->GetHash()>=item->GetHash()){
						item->next=_First;
						_First = item;
						return item;
				}

				if(_Last->GetHash()<=item->GetHash()){
						item->prev=_Last;
						_Last = item;
						return item;
				}

				// вставляем в середину списка
				HashItem<T> * zz;
				zz = _First;
				while(zz && zz->next){
						if(zz->GetHash()<= item->GetHash() && zz->next->GetHash()>item->GetHash()){
								item->prev=zz;
								item->next =zz->next;
								zz->next->prev=item;
								zz->next=item;
								return item;
						}
						zz=zz->next;
				}

		}

		HashItem<T> *Current(){ return _Cur};

		HashItem<T> *FindFirst(char * Name){
		 unsigned long hv= MakeHash(Name);
		 First();
		 while(_Cur){
				if(_Cur->GetHashValue()==hv){
							HashValue =hv;
						 return _Cur;
				}

				if(_Cur->GetHashValue()>hv)
						break;
				Next();
		 }
		 HashValue=0;
		 return _Cur;
		};

		HashItem<T> *FindNext(){
		 Next();
		 if(_Cur){
				if(_Cur->GetHashValue()==HashValue){
						 return _Cur;
				}
		 }
		 HashValue=0;
		 _Cur=0
		 return _Cur;
		};

		HashItem<T> *FindPrev(){
		 Prev();
		 if(_Cur){
				if(_Cur->GetHashValue()==HashValue){
						 return _Cur;
				}
		 }
		 HashValue=0;
		 _Cur=0
		 return _Cur;

		};
		HashItem<T> *FindLast(char * Name){
		 unsigned long hv= MakeHash(Name);
		 Last();
		 while(_Cur){
				if(_Cur->GetHashValue()==hv){
							HashValue =hv;
						 return _Cur;
				}

				if(_Cur->GetHashValue()>hv)
						break;
				Prev();
		 }
		 HashValue=0;
		 return _Cur;
		};

		HashItem<T> *First(){
		 _Cur=_First;
		 return _Cur;
		};

		HashItem<T> *Next(){
		 if(_Cur) _Cur=_Cur->next;
		 return _Cur;
		};

		HashItem<T> *Prev(){
		 if(_Cur) _Cur=_Cur->prev;
		 return _Cur;
		};

		HashItem<T> *Last(){
		 _Cur=_Last;
		 return _Cur;
		};

};

//
//////////////////////////////////////////////////////////////////////////////////////
template <class T> class HashArrayItem {
		long HashValue;
		T* ptr;
public:

		T * GetPtr(){ return ptr; }
		unsigned long GetHash(){ return HashValue; }
		HashArrayItem(T * _ptr,unsigned long Value){
				HashValue =Value;
				ptr=_ptr;
		};
};


template <class T> class MyHashArray: public MyArray <HashArrayItem<T> >{
		unsigned long HashValue;
		long _Cur;

		virtual unsigned long MakeHash(char *  Name){
				unsigned long hv=0;
				for(short i=0;i<3 ;i++){
						hv<<8;
						if(i<strlen(Name))
							hv+=(unsigned char) Name[i];
				}
				return hv;
		}

public:

		MyHashArray(bool _AutoDelete=false){
			HashValue=0;
			_Cur=0;
			DeleteOrder(_AutoDelete);
		};

		void Del(T * ptr, char* Name){
				HashArrayItem<T> *zz=FindFirst(Name);
				unsigned long hv=MakeHash(Name);

				while(zz){
					if(zz->GetPtr()==ptr &&  hv==zz->GetHash()){
						Destroy(zz);
					}
					zz=FindNext();
				}

		};

		HashArrayItem<T> * AddItem(T * ptr, char* Name){
				HashArrayItem<T> *item = new HashArrayItem<T>(ptr,MakeHash(Name));

				if(!Count()){
						Add(item);
						return item;
				}

				// проверяем края
				if(First()->GetHash()>=item->GetHash()){
						Insert(item,0);
						return item;
				}

				if(Last()->GetHash()<=item->GetHash()){
						Add(item);
						return item;
				}

				// вставляем в середину
				HashArrayItem<T> * zz,*zn;
				long idx;
				for(idx=0;idx<Count()-1;idx++){
						zz=Get(idx);
						zn=Get(idx+1);

						if(zz->GetHash() <= item->GetHash() && zn->GetHash()>item->GetHash()){
								Insert(item,idx+1);
								return item;
						}
				}
		}

		
		HashArrayItem<T> * ReHash(T * ptr, char* OldName, char * Name){
								bool aa =IsAutoDelete();
								deleteOrder(false);
								Del(ptr,OldName);
								deleteOrder(aa);
								return AddItem(ptr,Name);
		};


		HashArrayItem<T> *Current(){if(_Cur>=0) return Get(_Cur); else return 0;};

		HashArrayItem<T> *FindFirst(char * Name){
		 unsigned long hv= MakeHash(Name);
		 long cc=Count()/2;
		 long cn=0;
		 unsigned long lvl=4;
		 short stop=0;
		 if(!Count()) return 0;
		 
		 while(stop<2 && Get(cc)->GetHash()!=hv){
				cn=cc;
				long d=Count()/lvl;
				if(!d){d=1; stop++;}
				lvl*=2;
										
				if(Get(cc)->GetHash()>hv){
							if(cc-d<0) break;
							cc=cc-d;
							continue;
				}

				if(Get(cc)->GetHash()<hv){
							if(cc+d >= Count()) break;
							cc=cc+d;
							continue;
				}
				
				if(Get(cc)->GetHash()==hv) break;
		 }
		 
		 if(!Get(cc) || Get(cc)->GetHash()!=hv){
			 HashValue=0;
			 _Cur=-1;
			 return 0;
		 }

		 while(cc>=0 && Get(cc) && Get(cc)->GetHash()==hv){
			cc--;
		 }
		 _Cur=cc+1;
		 HashValue=Get(_Cur)->GetHash();
		 return Get(_Cur);
		};

		HashArrayItem<T> *FindNext(){
		 Next();
		 if(_Cur>=0){
				if(Get(_Cur)->GetHash()==HashValue){
						 return Get(_Cur);
				}
		 }
		 HashValue=0;
		 _Cur=-1;
		 return 0;
		};

		HashArrayItem<T> *FindPrev(){
		 Prev();
		 if(_Cur>=0)
				if(Get(_Cur)->GetHash()==HashValue){
						 return Get(_Cur);
				}
		 HashValue=0;
		 _Cur=-1;	 
		 return Get(_Cur);
		};
		
		HashArrayItem<T> *FindLast(char * Name){
		 unsigned long hv= MakeHash(Name);
		 long cc=Count()/2;
		 long cn=0;

		 unsigned long lvl=4;

		 while(cn!=cc && Get(cc)->GetHash()!=hv){
				cn=cc;
				if(Get(cc)->GetHash()>hv){
							cc=cc-Count()/lvl;
							
				}

				if(Get(cc)->GetHash()<hv){
							cc=cc+Count()/lvl;
							cn=cc;
				}
				if(Get(cc)->GetHash()==hv) break;
				lvl*=2;
		 }
		 if(!Get(cc) && Get(cc)->GetHash()!=hv){
			 HashValue=0;
			 _Cur=-1;
			 return 0;
		 }

		 while(cc<Count() && Get(cc) && Get(cc)->GetHash()==hv){
			cc++;
		 }
		 _Cur=cc-1;
		 HashValue=Get(_Cur)->GetHash();
		 return Get(_Cur)
		};

		HashArrayItem<T> *First(){
			if(Count()){
			 _Cur=0;
			 return Get(_Cur);
			}else{
			_Cur=-1;
			 return 0;
			} 
		};

		HashArrayItem<T> *Next(){
			if(_Cur+1<Count()) {
			 _Cur++;
			 return Get(_Cur);
			}else{
			_Cur=-1;
			 return 0;
			} 				};

		HashArrayItem<T> *Prev(){
		 if(_Cur-1>=0) {
			 _Cur--;
			 return Get(_Cur);
			}else{
			_Cur=-1;
			 return 0;
			}
		};

		HashArrayItem<T> *Last(){
			if(Count()){
			 _Cur=Count()-1;
			 return Get(_Cur);
			}else{
			_Cur=-1;
			 return 0;
			}
		};
};



//===========================================
template  <class T > class  MySortedArray {
	T **info;
	long size;
	long total;
	long delta;
	bool autoDelete;
 public:
  T* Clone(){ return new T(); };
	MySortedArray(long _Delta = 100,bool _autoDelete = true){
			info=0; size =0; delta =_Delta; total = 0;autoDelete = _autoDelete;
	};
	~MySortedArray(){
		Clear();
	 };

	void SetDelta(long _delta){
		if(_delta>0) delta=_delta;
		else delta =1;
	};

	void DeleteOrder(bool _autoDelete){
		autoDelete = _autoDelete;
	};

	bool IsAutoDelete(){
		return autoDelete;
	};

	T* Get(long idx) const;
	void Erase(long idx);
	long Add (T *v);
	void Clear(void);
	long Where(T *v);
	void Destroy(T *v){ long id=Where(v); if(size>0  && id>=0)Erase(id); };
	void Destroy(long id){ Erase(id); };
	long Count() const { return size;};
	T* operator [] (long idx) const { return Get(idx);};
	T* operator [] (int idx) const { return Get(idx);};

  long Find(const T*v);
  long FindLE(const T*v);
  long FindGE(const T*v);
  long FindGT(const T*v);
  long FindLT(const T*v);

  
	friend SimpleStream & operator << (SimpleStream & s, MySortedArray<T>& x){
	s << x.Count();
	for(long i=0;i< x.Count();i++){
		s << *(x.Get(i));
	}
	return s;
	};

 friend SimpleStream & operator >> (SimpleStream & s, MySortedArray<T>& x){
	long cnt;
	s >> cnt;
	x.Clear();
	for(long i=0;i<cnt;i++){
		x.Add( x.Clone());
		s >> *(x.Get(i));
	}
	return s;
	}	;
	
};


template <class T> long MySortedArray<T>::Find(const T *v){
  if(!size)
    return -1;
  if(!v)
    return -1;

  long step=2;
  long i=size/step;
  long delta=size;
  bool sgn;

  while( 0<=i && i<size && delta >=1){
    if(*info[i]==*v)
      return i;
    step=step*2;
    delta=size/step;
    if(*info[i]<*v){
      sgn = true;
      i=i+delta;
    }else{
      sgn = false;
      i=i-delta;
    }
  };

  while( 0<=i && i<size){
    if(*info[i]==*v)
      return i;

    if(*info[i]<*v && !sgn){
      return -1;
    }

    if(*v<*info[i] && sgn){
      return -1;
    }

    if(sgn){
      i=i+1;
    }else{
      i=i-1;
    }


  };

  return -1 ;
};

template <class T> long MySortedArray<T>::FindLE(const T*v){
  if(!size)
    return -1;
  if(!v)
    return -1;

  long step=2;
  long i=size/step;
  long delta=size;
  bool sgn;

  while( 0<=i && i<size && delta >=1){
    if(*info[i]==*v)
      return i;
    step=step*2;
    delta=size/step;
    if(*info[i]<*v){
      sgn = true;
      i=i+delta;
    }else{
      sgn = false;
      i=i-delta;
    }
  };

  while( 0<=i && i<size){
    if(*info[i]==*v)
      return i;

    if(*info[i]<*v && !sgn){
      return i;
    }

    if(*v<*info[i] && sgn){
      return -1;
    }

    if(sgn){
      i=i+1;
    }else{
      i=i-1;
    }
  };
  if(i==size)
    return size-1;
  return -1 ;
};
template <class T> long MySortedArray<T>::FindGE(const T*v){

  if(!size)
    return -1;
  if(!v)
    return -1;

  long step=2;
  long i=size/step;
  long delta=size;
  bool sgn;

  while( 0<=i && i<size && delta >=1){
    if(*info[i]==*v)
      return i;
    step=step*2;
    delta=size/step;
    if(*info[i]<*v){
      sgn = true;
      i=i+delta;
    }else{
      sgn = false;
      i=i-delta;
    }
  };

  while( 0<=i && i<size){
    if(*info[i]==*v)
      return i;

    if(*info[i]<*v && !sgn){
      return -1;
    }

    if(*v<*info[i] && sgn){
      return i;
    }

    if(sgn){
      i=i+1;
    }else{
      i=i-1;
    }


  };

  if(i<0)
    return 0;
  return -1 ;
};

template <class T> long MySortedArray<T>::FindGT(const T*v){
  long i = FindGE(v);
  while(i<size && *info[i]==*v) i++;
  if(i==size) return -1;
  return i;
};


template <class T> long MySortedArray<T>::FindLT(const T*v){
  long i = FindLE(v);
  while(i>=0 && *info[i]==*v) i--;
  return i;
};




template <class T> long MySortedArray<T>::Where(T *v){
		for(int i =0;i<size;i++)
			if (((void*)(info[i])) ==((void*) v))
				return i;
		return -1;
};

template <class T> 	void MySortedArray<T>::Erase(long cur){
	if ( cur >= 0 && cur < size){
		if(autoDelete){
				if( info ){
					ifdel (info[cur]);
				}
		}
		info[cur]=0;
	int i;
	for (i=cur+1;i<size;i++)
		info[i-1]=info[i];
	size --;
	}
};


template <class T>	T* MySortedArray<T>::Get(long idx) const{
		if (idx>=0 && idx <size)
			return info[idx];
		else{
			return 0;
			}
};

template <class T> long MySortedArray<T>::Add (T* v){
    if(!v) return -1;
    long id =FindGE(v);
    if(id>=0){
       if (total==size){
        T **info2;
        info2 = new T* [total+delta];
        memset(info2,0,sizeof(T*) *(total+delta));
		int i;
        for ( i = 0; i<id;i++)
          info2[i] = info[i];
        for ( i = id+1; i<size+1;i++)
          info2[i] = info[i-1];
        info2[id] = v;
        if(info) delete [] info;
        info = info2;
        total+=delta;
        size++;
        return id;
       }else{
        for (int i = size; i>=id+1;i--)
          info[i] = info[i-1];
        info[id] = v;
        size++;
        return id;
       }

    }else
      if (total==size){
        T **info2;
        info2 = new T* [total+delta];
          memset(info2,0,sizeof(T*) *(total+delta));
        for (int i = 0; i<size;i++)
          info2[i] = info[i];
        info2[size++] = v;
        if(info) delete [] info;
        info = info2;
        total+=delta;
        return size-1;
      } else{
        info[size] = v;
        size++;
        return size-1;
       }
};

template <class T>  void MySortedArray<T>::Clear(void){

		for (int i = 0; i<size; i++){
			if(autoDelete){
					if(info && info[i] ) {
						delete info[i];
					}
			}
		}
		if (info) delete [] info;
		total=size=0;
		info = 0;
};

#endif


