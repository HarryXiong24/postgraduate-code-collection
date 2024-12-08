import React, { useEffect, useState } from 'react';
import type { AppProps } from 'next/app';
import { AppOutline, HistogramOutline } from 'antd-mobile-icons';
import { useRouter } from 'next/navigation';
import { Footer, NavBar, SpinLoading, Tabs } from 'antd-mobile';
import styles from './app.module.scss';
import '@/styles/globals.css';

const App = ({ Component, pageProps }: AppProps) => {
  const router = useRouter();
  const [tab, setTab] = useState('control');

  const [loading, setLoading] = useState(true);

  useEffect(() => {
    const pathname = window && window.location.pathname;
    setTab(pathname === '/analyze' ? 'analyze' : 'control');
    setTimeout(() => {
      setLoading(false);
    }, 500);
  }, []);

  return (
    <div className={styles.home}>
      <header className={styles.header}>
        <NavBar back={null}>
          <p className={styles.title}>Smart Light</p>
        </NavBar>
      </header>
      <div>
        <Tabs
          activeKey={tab}
          onChange={(key: string) => {
            setTab(key);
            if (key === 'control') {
              router.push('/');
            } else {
              router.push('/analyze');
            }
          }}
        >
          <Tabs.Tab
            title={
              <div className={styles.tab}>
                <AppOutline fontSize={24} />
                <p style={{ paddingLeft: '0.3rem' }}>Control</p>
              </div>
            }
            key='control'
          ></Tabs.Tab>
          <Tabs.Tab
            title={
              <div className={styles.tab}>
                <HistogramOutline fontSize={24} />
                <p style={{ paddingLeft: '0.3rem' }}>Analyze</p>
              </div>
            }
            key='analyze'
          ></Tabs.Tab>
        </Tabs>
      </div>
      <main className={styles.main}>
        {loading ? (
          <SpinLoading color='primary' style={{ marginTop: '10rem', '--size': '48px' }} />
        ) : (
          <Component {...pageProps} />
        )}
      </main>
      <footer className={styles.footer}>
        <Footer content='@ 2024-present Smart Light All rights reserved'></Footer>
      </footer>
    </div>
  );
};

export default App;
