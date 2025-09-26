import { useEffect, useState } from 'react';
import { Button } from '@/components/ui/button';
import { Input } from '@/components/ui/input';
import { Label } from '@/components/ui/label';
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from '@/components/ui/card';
import { callNative, subscribeNativeEvents } from '@/lib/native';
import { LogIn, Send, User, Lock, Mail, Key, Gift, Eye, EyeOff } from 'lucide-react';
import logoUrl from '@/assets/logo.svg';

type Conversation = { id: number; title: string; lastMessage?: string };
type Member = { id: number; name: string; email?: string; avatar?: string };
type Message = { id: number; sender_id: number; text: string; created_at?: string };

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

  // Force light theme
  useEffect(() => {
    document.documentElement.classList.remove('dark');
  }, []);

  useEffect(() => {
    return subscribeNativeEvents((evt) => {
      try {
        if (evt && typeof evt === 'object' && String(evt.type || '').toLowerCase().includes('message')) {
          if (activeId != null) api.getHistory(activeId).then((r:any) => setMessages(r.messages || []));
        }
      } catch {}
    });
  }, [activeId]);

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
    setActiveId(id);
    if (!membersCache.has(id)) {
      const mem = await api.getMembers(id);
      setMembersCache((prev: Map<number, Member[]>) => new Map(prev).set(id, mem.members || mem || []));
    }
    const hist = await api.getHistory(id);
    setMessages(hist?.messages || []);
  }

  return (
    <div className="h-screen w-screen grid" style={{gridTemplateRows: '1fr'}}>
      <div className="ambient-glow" aria-hidden>
        <span className="glow"></span>
        <span className="glow"></span>
        <span className="glow"></span>
      </div>
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
        <div className="grid grid-cols-[280px_1fr] h-full">
          <aside className="border-r flex flex-col">
            <div className="p-3 font-medium">会话</div>
            <div className="flex-1 overflow-auto">
              {convos.map((c: Conversation) => (
                <button
                  key={c.id}
                  onClick={() => selectConversation(c.id)}
                  className={`w-full text-left px-3 py-2 border-b hover:bg-accent transition-transform ${activeId===c.id? 'bg-accent': ''}`}
                >
                  <div className="text-sm font-medium">{c.title}</div>
                  {c.lastMessage && <div className="text-xs text-muted-foreground truncate">{c.lastMessage}</div>}
                </button>
              ))}
              {!convos.length && <div className="p-3 text-sm text-muted-foreground">暂无会话</div>}
            </div>
          </aside>
          <main className="grid grid-rows-[auto_1fr_auto]">
            <div className="border-b px-4 py-2 flex items-center gap-2">
              <div className="font-medium">{convos.find(c=>c.id===activeId)?.title || '未选择会话'}</div>
            </div>
            <div className="overflow-auto p-3 space-y-2">
              {messages.map((m: Message) => (
                <div key={m.id} className="max-w-[72ch]">{m.text}</div>
              ))}
              {!messages.length && <div className="text-sm text-muted-foreground">暂无消息</div>}
            </div>
            <form className="border-t p-2 flex items-center gap-2" onSubmit={(e)=>{e.preventDefault(); setDraft('');}}>
              <Input value={draft} onChange={e=>setDraft(e.target.value)} placeholder="输入消息…"/>
              <Button type="submit" className="gap-1"><Send className="w-4 h-4"/>发送</Button>
            </form>
          </main>
        </div>
      )}
    </div>
  );
}

export default App;
