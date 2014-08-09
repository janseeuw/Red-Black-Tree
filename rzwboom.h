#include <cstdlib>
#include <iostream>
using std::ostream;
using std::max;
#include <queue>
using namespace std;

/**********************************************************************
 
 klasse: Rood-zwarte boom
 
 ***************************************************************************/

template <class T,class D>
class Rzwboom;

template <class T,class D>
class Rzwknoop;

template <class T,class D>
class Rzwboom{
    friend class Rzwknoop<T,D>;
public:
    Rzwboom<T,D>(){
        k=0;
    }
    ~Rzwboom<T,D>(){
        delete k;
    }
    D* zoekdata(const T&);
    void voegtoe(const T&,const D&);
    int diepte();
    int zwartediepte();
    bool controleerZwartediepte();
    friend ostream& operator<<(ostream& os,Rzwboom<T,D>& b){
        b.schrijf(os);
        return os;
    }
private:
    void roteer(bool naarlinks);
    void zoekknoop(const T&,Rzwboom<T,D>*& plaats);
    void voegtoe(Rzwboom<T,D>* plaats, const T& sl, const D& data, int sw);
    bool isRood();
    void schrijf(std::ostream&) const;
    void schrijf(std::ostream& os,Rzwknoop<T,D>* kn) const;
    Rzwknoop<T,D>* k;
};

/**********************************************************************
 
 klasse: Rood-zwarte knoop
 
 ***************************************************************************/


template <class T,class D>
class Rzwknoop{
    friend class Rzwboom<T,D>;
public:
    T sl;
    D data;
    bool rood;
    Rzwboom<T,D> links,rechts;
    Rzwknoop<T,D>(const T& _sl,const D& _data):sl(_sl),data(_data),rood(true){}
};

/**********************************************************************
 
 Implementatie
 
 ***************************************************************************/

/**************
 
 Roteer
 
 ***************/
template <class T,class D>
void Rzwboom<T,D>::roteer(bool naarlinks){
    Rzwknoop<T,D>* op;
    Rzwknoop<T,D>* neer=k;
    if (naarlinks){
        op=k->rechts.k;
        neer->rechts.k=op->links.k;
        op->links.k=neer;
    }
    else{
        op=k->links.k;
        neer->links.k=op->rechts.k;
        op->rechts.k=neer;
    }
    k=op;
    neer=0;
    op=0;
}


/**************
 
 Zoeken
 
 ***************/
template <class T,class D>
D* Rzwboom<T,D>::zoekdata(const T& sl){
    Rzwboom<T,D>* plaats;
    zoekknoop(sl,plaats);
    if (plaats->k==0)
        return 0;
    else return &(plaats->k->data);
}


template <class T,class D>
void Rzwboom<T,D>::zoekknoop(const T& sl,Rzwboom<T,D>*& plaats){
    plaats=this;
    while (plaats->k!=0 && plaats->k->sl!=sl){
        if (sl<plaats->k->sl)
            plaats=&(plaats->k->links);
        else
            plaats=&(plaats->k->rechts);
    }
}

/**************
 
 Toevoegen
 
 ***************/
template <class T,class D>
bool Rzwboom<T,D>::isRood(){
    if(k == 0){
        return false;
    }else{
        return k->rood;
    }
}

template <class T,class D>
void Rzwboom<T,D>::voegtoe(const T& sl,const D& data){
    voegtoe(this, sl, data, 0);
    k->rood=false;
}

template <class T,class D>
void Rzwboom<T,D>::voegtoe(Rzwboom<T,D>* h, const T& sl,const D& data, int sw){
    if(h->k==0){h->k = new Rzwknoop<T, D>(sl,data); return; }
    if(h->k->links.isRood() && h->k->rechts.isRood()){
        h->k->rood=true;
        h->k->links.k->rood=false;
        h->k->rechts.k->rood=false;
    }
    if(sl < h->k->sl){
        voegtoe(&(h->k->links), sl, data, 0);
        if(h->isRood() && h->k->links.isRood() && sw) h->roteer(false);
        if(h->k->links.isRood() && h->k->links.k->links.isRood()){
            h->roteer(false); h->k->rood=false; h->k->rechts.k->rood = true;
        }
    }else{
        voegtoe(&(h->k->rechts), sl, data, 1);
        if(h->isRood() && h->k->rechts.isRood() && !sw) h->roteer(true);
        if(h->k->rechts.isRood() && h->k->rechts.k->rechts.isRood()){
            h->roteer(true); h->k->rood=false; h->k->links.k->rood = true;
        }
    }
}


/**************
 
 Extra's
 
 ***************/
template <class T,class D>
int Rzwboom<T,D>::diepte(){
    if (k==0)
        return 0;
    else
        return max(k->links.diepte(),k->rechts.diepte())+1;
}

template <class T,class D>
int Rzwboom<T,D>::zwartediepte(){
    if (k == 0)
        return 1;
    
    int linksZwartediepte = k->links.zwartediepte();
    if (linksZwartediepte == 0)
        return linksZwartediepte;
    
    int rechtswartediepte = k->rechts.zwartediepte();
    if (rechtswartediepte == 0)
        return rechtswartediepte;
    
    if (linksZwartediepte != rechtswartediepte)
        return -1;
    else
        return linksZwartediepte + (isRood() ? 0 : 1);
}

template<class T, class D>
bool Rzwboom<T, D>::controleerZwartediepte() {
    if (k == 0)
        return true;
    
    if (!k->links.zwartediepte())
        return false;
    
    if (!k->rechts.zwartediepte())
        return false;
    
    if ((isRood() && k->links.isRood()) || k->rechts.isRood())
        return false;
    
    return true;
}

template <class T,class D>
void Rzwboom<T,D>::schrijf(ostream& os) const{
    if (k!=0){
        std::queue<Rzwknoop<T,D>*, std::deque<Rzwknoop<T,D>*> > q;
        q.push(k);
        while(!q.empty()){
            Rzwknoop<T,D>* nu=q.front();
            schrijf(os,nu);
            os<<" links: ";
            schrijf(os,nu->links.k);
            os<<" rechts: ";
            schrijf(os,nu->rechts.k);
            os<<std::endl;
            if (nu->links.k!=0)
                q.push(nu->links.k);
            if (nu->rechts.k!=0)
                q.push(nu->rechts.k);
            q.pop();
        }
    }
    else{
        schrijf(os,k);
    }
}

template <class T,class D>
void Rzwboom<T,D>::schrijf(ostream& os ,Rzwknoop<T,D>* kn) const{
    if (kn!=0)
        os<<
        (kn->rood?
         "rood ":"zwart")
        <<"("<<kn->sl<<","<<kn->data<<")";
    else
        os<<"---";
}





