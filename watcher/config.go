package watcher

type config struct {
    Fix  []Item
    Cwd  []Item
    Server Server
}

type Item struct {
    Online string
    Offline string
    exts string 
    Address string
    Port   int
    Auto  Bool
}
type Server struct{
     Address  string
     Poert    int
}


