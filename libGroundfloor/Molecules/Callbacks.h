
#ifndef __GFCallbacks_H
#define __GFCallbacks_H

#include "../Atoms/Callable.h"
#include "Vector.h"

#define GFCreateNotify(a,b,c,d)    new NotifyFunctionPointer<a,b>( c, d )
#define GFCreateRetreive(a,b,c,d)  new RetreiveFunctionPointer<a,b>( c, d )

namespace Groundfloor {

	// template implementaties moeten helaas in de header file, omdat iedere
	//  keer bij een nieuw type bij [new NotifyFunction<type>()] een nieuwe implementatie
	//  moet worden aangemaakt en worden gecompileerd in de uiteindelijke binary.

	template <class P>
	class NotifyAbstract : public Callable
	{
	protected:
		P aDefaultParam;
	public:
		NotifyAbstract() : Callable() {
			aDefaultParam = 0;
		}
		~NotifyAbstract() {
		}

		void setDefaultParam(P aParam) {
			aDefaultParam = aParam;
		}

		virtual void execute(P aParam) = 0;
	};

	template <class P>
	class RetreiveAbstract : public Freeable
	{
	public:
		RetreiveAbstract() : Freeable() {
		}
		~RetreiveAbstract() {
		}

		virtual P retreive() = 0;
	};



	template <class P>
	class MultiNotify : public NotifyAbstract<P>
	{
	protected:
		Groundfloor::Vector *aNotifyList;

		P aDefaultParam;
	public:
		MultiNotify() : NotifyAbstract<P>::NotifyAbstract() {
			aNotifyList = new Vector();
			aNotifyList->autoClear = true;
			aDefaultParam = 0;
		}

		MultiNotify(MultiNotify<P> *pNotify) : NotifyAbstract<P>::NotifyAbstract() {
			aNotifyList = new Vector();
			pNotify->aNotifyList->fastCopy(aNotifyList);
			aNotifyList->autoClear = false;
			aDefaultParam = pNotify->aDefaultParam;
		}

		~MultiNotify() {
			delete aNotifyList;
		}

		void setDefaultParam(P aParam) {
			aDefaultParam = aParam;
		}

		void execute() {
			execute(aDefaultParam);
		}

		void execute(P aParam) {
			unsigned int c = aNotifyList->size();
			for (unsigned int i = 0; i < c; i++) {
				NotifyAbstract<P> *obj = static_cast<NotifyAbstract<P> *>(aNotifyList->elementAt(i));
				if (obj != 0) {
					obj->execute(aParam);
				}
			}
		}

		void addNotify(NotifyAbstract<P> *aNotify) {
			aNotifyList->addElement(aNotify);
		}

		void clearNotifies() {
			aNotifyList->clear();
		}

		void removeNotify(NotifyAbstract<P> *aNotify) {
			aNotifyList->removeElement(aNotify);
		}

		NotifyAbstract<P> *getNotify(unsigned int i) {
			return static_cast<NotifyAbstract<P> *>(aNotifyList->elementAt(i));
		}

		unsigned int getNotifyCount() {
			return aNotifyList->size();
		}

		// operators
		MultiNotify& operator+= (NotifyAbstract<P> *aNotify) {
			aNotifyList->addElement(aNotify);
			return *this;
		}
	};


	template <class P, class T>
	class NotifyFunctionPointer : public NotifyAbstract<P>
	{
	protected:
		typedef void (T::*FuncCallbackOneParam)(P);

		T *pObject;
		FuncCallbackOneParam pFunction;

	public:
		NotifyFunctionPointer(T *pObject, FuncCallbackOneParam pFunction) : NotifyAbstract<P>::NotifyAbstract() {
			this->pObject = pObject;
			this->pFunction = pFunction;
		};

		~NotifyFunctionPointer() {
		};

		void execute() {
			(pObject->*pFunction)(this->aDefaultParam);
		};

		void execute(P aParam) {
			(pObject->*pFunction)(aParam);
		};
	};

	template <class P, class T>
	class RetreiveFunctionPointer : public RetreiveAbstract<P>
	{
	protected:
		typedef P(T::*FuncCallbackNoParam)();

		T *pObject;
		FuncCallbackNoParam pFunction;

	public:
		RetreiveFunctionPointer(T *pObject, FuncCallbackNoParam pFunction) : RetreiveAbstract<P>::RetreiveAbstract() {
			this->pObject = pObject;
			this->pFunction = pFunction;
		};

		~RetreiveFunctionPointer() {
		};

		P retreive() {
			return (pObject->*pFunction)();
		};
	};


}

#endif // __GFCallbackPointerClasses_H
