import { useEffect, useState, useRef } from 'react';
import { Button } from '@/components/ui/button';
import { Input } from '@/components/ui/input';
import { Label } from '@/components/ui/label';
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from '@/components/ui/card';
import { callNative, subscribeNativeEvents } from '@/lib/native';
import { LogIn, Send, User, Lock, Mail, Key, Gift, Eye, EyeOff } from 'lucide-react';
import logoUrl from '@/assets/logo.svg';
import Modal from '@/components/Modal';

type Conversation = { id: number; title: string; lastMessage?: string };
type Member = { id: number; name: string; email?: string; avatar?: string };
type Message = { id: number; sender_id: number; text: string; created_at?: string; timestamp?: Date };

function useNativeAPI() {
  const signIn = (username: string, password: string) => callNative('SignIn', username, password);
  const getConversations = () => callNative('GetConversationList');
  const getMembers = (conversationId: number) => callNative('GetConversationMemberList', conversationId);
  const getHistory = (conversationId: number) => callNative('GetMessageHistory', conversationId);
  const findUsers = (q: string) => callNative('FindUsers', q);
  const addFriend = (id: number) => callNative('AddFriend', id);
  const getVerificationCode = (email: string) => callNative('getVerificationCode', email);
  return { signIn, getConversations, getMembers, getHistory, findUsers, addFriend, getVerificationCode };
}

function App() {
  const api = useNativeAPI();
  const [stage, setStage] = useState<'login' | 'register' | 'chat'>('login');
  const [busy, setBusy] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const [shakeLogin, setShakeLogin] = useState(false);
  const [showPwd, setShowPwd] = useState(false);
  const [showRegPwd, setShowRegPwd] = useState(false);
  const [regBusy, setRegBusy] = useState(false);
  const [regError, setRegError] = useState<string | null>(null);
  const [regMsg, setRegMsg] = useState<string | null>(null);
  const [cooldown, setCooldown] = useState(0);
  const [, setMe] = useState<{ id: number; name: string } | null>(null);
  const [convos, setConvos] = useState<Conversation[]>([]);
  const [activeId, setActiveId] = useState<number | null>(null);
  const [membersCache, setMembersCache] = useState<Map<number, Member[]>>(new Map());
  const [messages, setMessages] = useState<Message[]>([]);
  const [draft, setDraft] = useState('');
  const [myUserId, setMyUserId] = useState<number | null>(null);
  const messagesEndRef = useRef<HTMLDivElement>(null);
  const inputRef = useRef<HTMLInputElement>(null);
  const [sendingMessage, setSendingMessage] = useState(false);
  const [loadingConversation, setLoadingConversation] = useState(false);
  const [isTransitioning, setIsTransitioning] = useState(false);
  const [showAddFriend, setShowAddFriend] = useState(false);
  const [friendSearchQuery, setFriendSearchQuery] = useState('');
  const [searchResults, setSearchResults] = useState<any[]>([]);
  const [searchingFriends, setSearchingFriends] = useState(false);
  const [addingFriend, setAddingFriend] = useState(false);

  // Force light theme
  useEffect(() => {
    document.documentElement.classList.remove('dark');
  }, []);

  useEffect(() => {
    return subscribeNativeEvents((evt) => {
      try {
        if (evt && typeof evt === 'object' && String(evt.type || '').toLowerCase().includes('message')) {
          if (activeId != null) {
            api.getHistory(activeId).then((r:any) => {
              setMessages(r.messages || []);
              setTimeout(() => {
                messagesEndRef.current?.scrollIntoView({ behavior: 'smooth' });
              }, 100);
            });
          }
        }
      } catch {}
    });
  }, [activeId]);

  // Auto-scroll when messages change
  useEffect(() => {
    if (messages.length > 0) {
      setTimeout(() => {
        messagesEndRef.current?.scrollIntoView({ behavior: 'smooth' });
      }, 100);
    }
  }, [messages]);

  // Keyboard navigation
  useEffect(() => {
    const handleKeyDown = (e: KeyboardEvent) => {
      // Focus input with Ctrl/Cmd + K
      if ((e.ctrlKey || e.metaKey) && e.key === 'k') {
        e.preventDefault();
        inputRef.current?.focus();
      }

      // Navigate conversations with Ctrl/Cmd + Up/Down
      if ((e.ctrlKey || e.metaKey) && (e.key === 'ArrowUp' || e.key === 'ArrowDown')) {
        e.preventDefault();
        const currentIndex = convos.findIndex(c => c.id === activeId);
        const newIndex = e.key === 'ArrowUp'
          ? Math.max(0, currentIndex - 1)
          : Math.min(convos.length - 1, currentIndex + 1);

        if (convos[newIndex]) {
          selectConversation(convos[newIndex].id);
        }
      }
    };

    document.addEventListener('keydown', handleKeyDown);
    return () => document.removeEventListener('keydown', handleKeyDown);
  }, [convos, activeId]);

  // Real-time friend search
  useEffect(() => {
    if (!friendSearchQuery.trim()) {
      setSearchResults([]);
      return;
    }

    const searchFriends = async () => {
      setSearchingFriends(true);
      try {
        const results = await api.findUsers(friendSearchQuery.trim());
        setSearchResults(results?.users || []);
      } catch (error) {
        console.error('Failed to search users:', error);
        setSearchResults([]);
      } finally {
        setSearchingFriends(false);
      }
    };

    const debounceTimer = setTimeout(searchFriends, 300);
    return () => clearTimeout(debounceTimer);
  }, [friendSearchQuery]);

  async function handleLogin(e: React.FormEvent<HTMLFormElement>) {
    e.preventDefault();
    const fd = new FormData(e.currentTarget);
    const username = String(fd.get('username') || '').trim();
    const password = String(fd.get('password') || '');
    if (!username || !password) { setError('请输入用户名与密码'); return; }
    setBusy(true); setError(null);
    try {
      const r: any = await api.signIn(username, password);
      if (r?.status === 'Success') {
        setMe({ id: r.user_id, name: username });
        setMyUserId(r.user_id);
        setIsTransitioning(true);

        // Start transition animation
        setTimeout(async () => {
          setStage('chat');
          const list: any = await api.getConversations();
          const convs: Conversation[] = (list?.conversations || []) as Conversation[];
          setConvos(convs);
          if (convs.length) {
            setActiveId(convs[0].id);
            const mem = await api.getMembers(convs[0].id);
            setMembersCache(new Map([[convs[0].id, mem.members || mem || []]]));
            const hist = await api.getHistory(convs[0].id);
            setMessages(hist?.messages || []);
          }
          // End transition after chat is loaded
          setTimeout(() => setIsTransitioning(false), 300);
        }, 800); // Delay to show transition effect
      } else {
        setError(r?.message || '登录失败');
        setShakeLogin(true); setTimeout(()=> setShakeLogin(false), 420);
      }
    } catch (e: any) {
      setError(e?.message || '发生错误');
      setShakeLogin(true); setTimeout(()=> setShakeLogin(false), 420);
    } finally {
      setBusy(false);
    }
  }

  async function handleGetCode(email: string) {
    if (!/^[^@\s]+@[^@\s]+\.[^@\s]+$/.test(email)) { setRegError('请填写有效邮箱'); return; }
    try {
      setRegError(null); setRegMsg(null);
      await api.getVerificationCode(email);
      setRegMsg('验证码已发送至邮箱');
      setCooldown(60);
    } catch (e: any) {
      setRegError(e?.message || '验证码发送失败');
    }
  }

  useEffect(() => {
    if (cooldown <= 0) return;
    const t = setInterval(()=> setCooldown((c:number)=>c-1), 1000);
    return () => clearInterval(t);
  }, [cooldown]);

  async function handleRegister(e: React.FormEvent<HTMLFormElement>) {
    e.preventDefault();
    const fd = new FormData(e.currentTarget);
    const username = String(fd.get('username')||'').trim();
    const email = String(fd.get('email')||'').trim();
    const invite = String(fd.get('invite')||'').trim();
    const code = String(fd.get('code')||'').trim();
    const password = String(fd.get('password')||'');
    if (!username || !email || !invite || !code || !password) { setRegError('请完整填写信息'); return; }
    setRegBusy(true); setRegError(null); setRegMsg(null);
    try {
      const r: any = await callNative('SignUp', username, invite, email, code, password);
      if (r?.success) {
        setRegMsg(r?.message || '注册成功，去登录');
        setTimeout(()=> setStage('login'), 800);
      } else {
        setRegError(r?.message || '注册失败');
      }
    } catch (e: any) {
      setRegError(e?.message || '发生错误');
    } finally {
      setRegBusy(false);
    }
  }

  async function selectConversation(id: number) {
    setLoadingConversation(true);
    setActiveId(id);
    try {
      if (!membersCache.has(id)) {
        const mem = await api.getMembers(id);
        setMembersCache((prev: Map<number, Member[]>) => new Map(prev).set(id, mem.members || mem || []));
      }
      const hist = await api.getHistory(id);
      setMessages(hist?.messages || []);
    } catch (error) {
      console.error('Failed to load conversation:', error);
    } finally {
      setLoadingConversation(false);
    }
  }

  async function handleAddFriend(userId: number) {
    setAddingFriend(true);
    try {
      const result = await api.addFriend(userId);
      if (result?.success) {
        // Close modal and show success feedback
        setShowAddFriend(false);
        setFriendSearchQuery('');
        setSearchResults([]);
        // You could show a toast notification here
      }
    } catch (error) {
      console.error('Failed to add friend:', error);
    } finally {
      setAddingFriend(false);
    }
  }

  function handleCloseAddFriend() {
    setShowAddFriend(false);
    setFriendSearchQuery('');
    setSearchResults([]);
  }

  return (
    <div className="h-screen w-screen grid relative overflow-hidden" style={{gridTemplateRows: '1fr'}}>
      {/* Unified Background for All Interfaces */}
      <div className="unified-background" aria-hidden>
        <div className="unified-orb unified-orb-1"></div>
        <div className="unified-orb unified-orb-2"></div>
        <div className="unified-orb unified-orb-3"></div>
        <div className="unified-orb unified-orb-4"></div>
        <div className="unified-orb unified-orb-5"></div>
      </div>

      {/* Transition Overlay */}
      {isTransitioning && (
        <div className="fixed inset-0 z-50 transition-overlay">
          <div className="absolute inset-0 bg-gradient-to-br from-blue-400/20 via-purple-500/20 to-pink-400/20 backdrop-blur-sm"></div>
          <div className="absolute inset-0 flex items-center justify-center">
            <div className="text-center text-white">
              <div className="w-16 h-16 border-4 border-white/30 border-t-white rounded-full animate-spin mx-auto mb-4"></div>
              <div className="text-lg font-medium">欢迎进入聊天</div>
              <div className="text-sm opacity-80 mt-1">正在为您准备界面...</div>
            </div>
          </div>
        </div>
      )}
      {/* No app header per request */}
      {stage === 'login' && (
        <div className="flex items-center justify-center p-6">
          <Card className={`w-full max-w-md anim-pop ${shakeLogin? 'shake' : ''}`}>
            <CardHeader className="text-center">
              <div className="flex justify-center mb-2"><img src={logoUrl} className="h-16 w-auto"/></div>
              <CardTitle className="flex items-center justify-center gap-2">登录</CardTitle>
              <CardDescription>使用账户登录以继续</CardDescription>
            </CardHeader>
            <CardContent>
              <form className="grid gap-4" onSubmit={handleLogin}>
                <div className="field anim-fade-up">
                  <Input id="username" name="username" placeholder=" " autoComplete="username" required className="peer"/>
                  <Label htmlFor="username" className="floating-label"><User className="icon w-4 h-4"/>用户名</Label>
                </div>
                <div className="field anim-fade-up">
                  <div className="relative">
                    <Input id="password" name="password" type={showPwd? 'text' : 'password'} placeholder=" " autoComplete="current-password" required className="peer pr-10"/>
                    <Label htmlFor="password" className="floating-label"><Lock className="icon w-4 h-4"/>密码</Label>
                    <button type="button" aria-label="切换密码可见" onClick={()=>setShowPwd(v=>!v)} className="absolute right-2 top-1/2 -translate-y-1/2 text-muted-foreground hover:text-foreground">
                      {showPwd ? <EyeOff className="w-4 h-4"/> : <Eye className="w-4 h-4"/>}
                    </button>
                  </div>
                </div>
                {error && <div className="text-sm text-red-600 anim-fade-up">{error}</div>}
                <Button type="submit" variant="brand" size="xl" disabled={busy} className={`gap-2 ${busy?'btn-busy':''}`}>
                  <LogIn className="w-5 h-5" />
                  {busy ? '登录中…' : '登录'}
                </Button>
                <div className="text-sm text-muted-foreground text-center">没有账号？
                  <button type="button" className="text-primary underline underline-offset-4 ml-1" onClick={()=>{ setStage('register'); setError(null); }}>注册</button>
                </div>
              </form>
            </CardContent>
          </Card>
        </div>
      )}
      {stage === 'register' && (
        <div className="flex items-center justify-center p-6">
          <Card className="w-full max-w-md anim-pop">
            <CardHeader className="text-center">
              <div className="flex justify-center mb-2"><img src={logoUrl} className="h-10 w-auto"/></div>
              <CardTitle className="flex items-center justify-center gap-2">注册</CardTitle>
              <CardDescription>创建新账户</CardDescription>
            </CardHeader>
            <CardContent>
              <form className="grid gap-4" onSubmit={handleRegister}>
                <div className="field anim-fade-up">
                  <Input id="r_username" name="username" placeholder=" " required className="peer"/>
                  <Label htmlFor="r_username" className="floating-label"><User className="icon w-4 h-4"/>用户名</Label>
                </div>
                <div className="field anim-fade-up">
                  <div className="relative">
                    <Input id="r_email" name="email" placeholder=" " type="email" className="peer pr-32" required/>
                    <Label htmlFor="r_email" className="floating-label"><Mail className="icon w-4 h-4"/>邮箱</Label>
                    <Button type="button" variant="outline" className="absolute right-1 top-1/2 -translate-y-1/2 h-9" disabled={cooldown>0} onClick={()=>{
                      const el = document.getElementById('r_email') as HTMLInputElement | null;
                      handleGetCode(el?.value || '');
                    }}>{cooldown>0? `${cooldown}s` : '获取验证码'}</Button>
                  </div>
                </div>
                <div className="field anim-fade-up">
                  <Input id="r_invite" name="invite" placeholder=" " required className="peer"/>
                  <Label htmlFor="r_invite" className="floating-label"><Gift className="icon w-4 h-4"/>邀请码</Label>
                </div>
                <div className="field anim-fade-up">
                  <Input id="r_code" name="code" placeholder=" " required className="peer"/>
                  <Label htmlFor="r_code" className="floating-label"><Key className="icon w-4 h-4"/>验证码</Label>
                </div>
                <div className="field anim-fade-up">
                  <div className="relative">
                    <Input id="r_password" name="password" type={showRegPwd? 'text' : 'password'} placeholder=" " required className="peer pr-10"/>
                    <Label htmlFor="r_password" className="floating-label"><Lock className="icon w-4 h-4"/>密码</Label>
                    <button type="button" aria-label="切换密码可见" onClick={()=>setShowRegPwd(v=>!v)} className="absolute right-2 top-1/2 -translate-y-1/2 text-muted-foreground hover:text-foreground">
                      {showRegPwd ? <EyeOff className="w-4 h-4"/> : <Eye className="w-4 h-4"/>}
                    </button>
                  </div>
                </div>
                {regError && <div className="text-sm text-red-600 anim-fade-up">{regError}</div>}
                {regMsg && <div className="text-sm text-green-600 anim-fade-up">{regMsg}</div>}
                <Button type="submit" variant="brand" size="xl" disabled={regBusy} className={`gap-2 ${regBusy?'btn-busy':''}`}>注册</Button>
                <div className="text-sm text-muted-foreground text-center">已有账号？
                  <button type="button" className="text-primary underline underline-offset-4 ml-1" onClick={()=>{ setStage('login'); setRegError(null); setRegMsg(null); }}>登录</button>
                </div>
              </form>
            </CardContent>
          </Card>
        </div>
      )}
      {stage === 'chat' && (
        <div className={`grid grid-cols-[320px_1fr] h-full transition-all duration-700 relative z-10 ${isTransitioning ? 'opacity-0 scale-95' : 'opacity-100 scale-100'}`}>
          <aside className="border-r border-gray-200 flex flex-col bg-white/60 backdrop-blur-md" role="navigation" aria-label="会话列表">
            <div className="p-4 border-b border-gray-200 bg-white/40">
              <div className="flex items-center justify-between">
                <div>
                  <h2 className="font-semibold text-gray-900 text-lg">会话</h2>
                  <p className="text-sm text-gray-600 mt-1">选择一个会话开始聊天</p>
                </div>
                <button
                  onClick={() => setShowAddFriend(true)}
                  className="p-2 bg-blue-500/20 hover:bg-blue-500/30 backdrop-blur-sm rounded-lg transition-all duration-200 group"
                  aria-label="添加好友"
                >
                  <User className="w-5 h-5 text-blue-600 group-hover:scale-110 transition-transform duration-200" />
                </button>
              </div>
            </div>
            <div className="flex-1 overflow-auto p-2" role="list">
              {convos.map((c: Conversation) => (
                <button
                  key={c.id}
                  onClick={() => selectConversation(c.id)}
                  className={`w-full text-left p-4 mb-2 rounded-xl transition-all duration-200 group hover:shadow-lg focus:outline-none focus:ring-2 focus:ring-blue-400 focus:ring-offset-2 ${
                    activeId===c.id
                      ? 'bg-blue-50 border border-blue-200 shadow-lg backdrop-blur-sm'
                      : 'bg-white/70 border border-gray-200 hover:bg-white/90 hover:border-gray-300 backdrop-blur-sm'
                  }`}
                  aria-current={activeId===c.id ? 'page' : undefined}
                  aria-label={`切换到${c.title}会话`}
                >
                  <div className="flex items-center space-x-3">
                    <div className="w-12 h-12 rounded-full bg-gradient-to-br from-blue-400 to-purple-500 flex items-center justify-center text-white font-semibold text-lg shadow-md">
                      {c.title.charAt(0).toUpperCase()}
                    </div>
                    <div className="flex-1 min-w-0">
                      <div className={`font-medium truncate ${
                        activeId===c.id ? 'text-blue-800' : 'text-gray-900'
                      }`}>{c.title}</div>
                      {c.lastMessage && (
                        <div className="text-sm text-gray-600 truncate mt-1">{c.lastMessage}</div>
                      )}
                      <div className="flex items-center justify-between mt-1">
                        <div className="text-xs text-gray-400">刚刚</div>
                        {activeId !== c.id && (
                          <div className="w-2 h-2 bg-blue-500 rounded-full opacity-0 group-hover:opacity-100 transition-opacity duration-200"></div>
                        )}
                      </div>
                    </div>
                  </div>
                </button>
              ))}
              {!convos.length && (
                <div className="p-8 text-center">
                  <div className="w-16 h-16 bg-gray-100 rounded-full flex items-center justify-center mx-auto mb-4">
                    <User className="w-8 h-8 text-gray-400" />
                  </div>
                  <div className="text-sm text-gray-600 mb-2">暂无会话</div>
                  <div className="text-xs text-gray-500">等待新的对话开始</div>
                </div>
              )}
            </div>
          </aside>
          <main className="grid grid-rows-[auto_1fr_auto]">
            <div className="border-b border-gray-200 px-6 py-4 bg-white/60 backdrop-blur-md flex items-center justify-between">
              <div className="flex items-center space-x-4">
                {activeId && (
                  <div className="w-10 h-10 rounded-full bg-gradient-to-br from-green-400 to-blue-500 flex items-center justify-center text-white font-semibold shadow-md">
                    {(convos.find(c=>c.id===activeId)?.title || '').charAt(0).toUpperCase()}
                  </div>
                )}
                <div>
                  <div className="font-semibold text-gray-900 text-lg">
                    {convos.find(c=>c.id===activeId)?.title || '未选择会话'}
                  </div>
                  {activeId && (
                    <div className="flex items-center text-sm text-gray-600">
                      <div className="w-2 h-2 bg-green-400 rounded-full mr-2 animate-pulse"></div>
                      在线
                    </div>
                  )}
                </div>
              </div>
              {activeId && (
                <div className="flex items-center space-x-2">
                  <button className="p-2 hover:bg-gray-100 rounded-lg transition-colors duration-200">
                    <svg className="w-5 h-5 text-gray-600" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                      <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M3 5a2 2 0 012-2h3.28a1 1 0 01.948.684l1.498 4.493a1 1 0 01-.502 1.21l-2.257 1.13a11.042 11.042 0 005.516 5.516l1.13-2.257a1 1 0 011.21-.502l4.493 1.498a1 1 0 01.684.949V19a2 2 0 01-2 2h-1C9.716 21 3 14.284 3 6V5z" />
                    </svg>
                  </button>
                  <button className="p-2 hover:bg-gray-100 rounded-lg transition-colors duration-200">
                    <svg className="w-5 h-5 text-gray-600" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                      <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M15 10l4.553-2.276A1 1 0 0121 8.618v6.764a1 1 0 01-1.447.894L15 14M5 18h8a2 2 0 002-2V8a2 2 0 00-2-2H5a2 2 0 00-2 2v8a2 2 0 002 2z" />
                    </svg>
                  </button>
                  <button className="p-2 hover:bg-gray-100 rounded-lg transition-colors duration-200">
                    <svg className="w-5 h-5 text-gray-600" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                      <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M12 5v.01M12 12v.01M12 19v.01M12 6a1 1 0 110-2 1 1 0 010 2zm0 7a1 1 0 110-2 1 1 0 010 2zm0 7a1 1 0 110-2 1 1 0 010 2z" />
                    </svg>
                  </button>
                </div>
              )}
            </div>
            <div className="overflow-auto p-3 space-y-3" style={{scrollBehavior: 'smooth'}} role="log" aria-label="聊天消息" aria-live="polite">
              <div className="flex flex-col space-y-3">
                {messages.map((m: Message) => {
                  const isMyMessage = m.sender_id === myUserId;
                  const timestamp = m.created_at || new Date().toLocaleTimeString('zh-CN', { hour: '2-digit', minute: '2-digit' });

                  return (
                    <div key={m.id} className={`flex ${isMyMessage ? 'justify-end' : 'justify-start'} animate-in slide-in-from-bottom-2 duration-300`}>
                      <div className={`group max-w-[70%] flex flex-col ${isMyMessage ? 'items-end' : 'items-start'}`}>
                        <div className={`px-4 py-3 rounded-2xl shadow-sm transition-all duration-200 hover:shadow-md ${
                          isMyMessage
                            ? 'bg-gradient-to-br from-blue-500 to-blue-600 text-white'
                            : 'bg-gray-100 text-gray-900 border border-gray-200'
                        }`}>
                          <p className="text-sm leading-relaxed break-words">{m.text}</p>
                        </div>
                        <div className="text-xs text-muted-foreground mt-1 px-1 opacity-0 group-hover:opacity-100 transition-opacity duration-200">
                          {timestamp}
                        </div>
                      </div>
                    </div>
                  );
                })}
                {loadingConversation && (
                  <div className="flex items-center justify-center h-32">
                    <div className="text-center">
                      <div className="w-8 h-8 border-3 border-gray-300 border-t-blue-500 rounded-full animate-spin mx-auto mb-3"></div>
                      <div className="text-sm text-muted-foreground">加载消息中...</div>
                    </div>
                  </div>
                )}
                {!loadingConversation && !messages.length && (
                  <div className="flex items-center justify-center h-32">
                    <div className="text-center">
                      <div className="text-sm text-muted-foreground mb-2">暂无消息</div>
                      <div className="text-xs text-muted-foreground">开始您的第一次对话吧！</div>
                    </div>
                  </div>
                )}
                <div ref={messagesEndRef} />
              </div>
            </div>
            <form className="border-t border-gray-200 p-4 flex items-center gap-3 bg-white/50 backdrop-blur-md" onSubmit={async (e)=>{
              e.preventDefault();
              if (!draft.trim() || !activeId || sendingMessage) return;

              setSendingMessage(true);
              const messageText = draft.trim();
              const newMessage: Message = {
                id: Date.now(),
                sender_id: myUserId || 0,
                text: messageText,
                created_at: new Date().toLocaleTimeString('zh-CN', { hour: '2-digit', minute: '2-digit' })
              };

              // Optimistically add message
              setMessages(prev => [...prev, newMessage]);
              setDraft('');

              // Simulate sending delay
              try {
                await new Promise(resolve => setTimeout(resolve, 500));
                // Here you would normally send to the server
              } catch (error) {
                console.error('Failed to send message:', error);
                // Remove the optimistic message on error
                setMessages(prev => prev.filter(m => m.id !== newMessage.id));
                setDraft(messageText); // Restore draft
              } finally {
                setSendingMessage(false);
              }

              // Scroll to bottom after adding message
              setTimeout(() => {
                messagesEndRef.current?.scrollIntoView({ behavior: 'smooth' });
              }, 100);
            }}>
              <div className="flex-1 relative">
                <Input
                  ref={inputRef}
                  value={draft}
                  onChange={e=>setDraft(e.target.value)}
                  placeholder={activeId ? "输入消息… (Ctrl+K 快速聚焦)" : "请选择一个会话"}
                  className="pr-12 h-11 rounded-full border-gray-200 focus:border-blue-400 focus:ring-blue-400/20"
                  disabled={!activeId || sendingMessage}
                  aria-label="消息输入框"
                  autoComplete="off"
                />
                <div className="absolute right-3 top-1/2 -translate-y-1/2 text-xs text-muted-foreground">
                  {sendingMessage ? (
                    <div className="flex items-center">
                      <div className="w-3 h-3 border border-gray-300 border-t-blue-500 rounded-full animate-spin mr-1"></div>
                      发送中
                    </div>
                  ) : (
                    draft.length > 0 && `${draft.length}`
                  )}
                </div>
              </div>
              <Button
                type="submit"
                size="lg"
                disabled={!draft.trim() || !activeId || sendingMessage}
                className="h-11 px-6 rounded-full bg-blue-500 hover:bg-blue-600 disabled:bg-gray-300 disabled:text-gray-500 transition-all duration-200 shadow-md hover:shadow-lg"
              >
                {sendingMessage ? (
                  <>
                    <div className="w-4 h-4 border-2 border-white border-t-transparent rounded-full animate-spin mr-2"></div>
                    发送中
                  </>
                ) : (
                  <>
                    <Send className="w-4 h-4 mr-2"/>发送
                  </>
                )}
              </Button>
            </form>
          </main>
        </div>
      )}

      {/* Add Friend Modal */}
      <Modal
        isOpen={showAddFriend}
        onClose={handleCloseAddFriend}
        title="添加好友"
        maxWidth="md"
      >
        <div className="relative mb-4">
          <Input
            value={friendSearchQuery}
            onChange={(e) => setFriendSearchQuery(e.target.value)}
            placeholder="搜索用户名或邮箱..."
            className="w-full pr-10 bg-white/70 border-gray-200 text-gray-900 placeholder:text-gray-500"
            autoFocus
          />
          {searchingFriends && (
            <div className="absolute right-3 top-1/2 -translate-y-1/2">
              <div className="w-4 h-4 border-2 border-gray-300 border-t-blue-500 rounded-full animate-spin"></div>
            </div>
          )}
        </div>

        <div className="max-h-64 overflow-y-auto">
          {searchResults.length > 0 ? (
            <div className="space-y-2">
              {searchResults.map((user: any, index: number) => (
                <div
                  key={user.id}
                  className="flex items-center justify-between p-3 bg-gray-50 rounded-lg border border-gray-200 hover:bg-gray-100 transition-all duration-200 animate-in slide-in-from-left-2 fade-in"
                  style={{animationDelay: `${index * 100}ms`}}
                >
                  <div className="flex items-center space-x-3">
                    <div className="w-10 h-10 rounded-full bg-gradient-to-br from-blue-400 to-purple-500 flex items-center justify-center text-white font-semibold">
                      {(user.name || user.username || 'U').charAt(0).toUpperCase()}
                    </div>
                    <div>
                      <div className="font-medium text-gray-900">{user.name || user.username}</div>
                      {user.email && (
                        <div className="text-sm text-gray-600">{user.email}</div>
                      )}
                    </div>
                  </div>
                  <Button
                    onClick={() => handleAddFriend(user.id)}
                    disabled={addingFriend}
                    size="sm"
                    className="bg-blue-500/80 hover:bg-blue-600/80 backdrop-blur-sm"
                  >
                    {addingFriend ? (
                      <>
                        <div className="w-3 h-3 border border-white border-t-transparent rounded-full animate-spin mr-1"></div>
                        添加中
                      </>
                    ) : (
                      '添加好友'
                    )}
                  </Button>
                </div>
              ))}
            </div>
          ) : friendSearchQuery.trim() && !searchingFriends ? (
            <div className="text-center py-8 text-gray-500 animate-in fade-in duration-300">
              <User className="w-12 h-12 text-gray-400 mx-auto mb-3" />
              <div className="font-medium">未找到用户</div>
              <div className="text-sm">请尝试其他搜索关键词</div>
            </div>
          ) : !friendSearchQuery.trim() ? (
            <div className="text-center py-8 text-gray-500 animate-in fade-in duration-300">
              <div className="font-medium">开始搜索</div>
              <div className="text-sm">输入用户名或邮箱来查找好友</div>
            </div>
          ) : null}
        </div>
      </Modal>
    </div>
  );
}

export default App;
