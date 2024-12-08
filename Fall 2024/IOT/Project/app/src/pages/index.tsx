import React, { useEffect, useState } from 'react';
import { NoticeBar, Switch } from 'antd-mobile';
import styles from './index.module.scss';
import { ExclamationTriangleOutline } from 'antd-mobile-icons';

const Control = () => {
  const [isOpen, setIsOpen] = useState(false);
  const [isError, setIsError] = useState(false);
  const [timeDifference, setTimeDifference] = useState('');
  const [currentTimeUsage, setCurrentTimeUsage] = useState(0);

  const getLightState = async () => {
    try {
      const res = await fetch('/api/control-light', {
        method: 'GET',
        headers: {
          'Content-Type': 'application/json',
        },
      });
      if (res.ok) {
        const result = await res.json();
        setIsOpen(result.isOpen);
        setCurrentTimeUsage(result.execTime);
        setIsError(false);
      }
    } catch (error) {
      setIsError(true);
      console.log(error);
    }
  };

  const handleState = async (checked: boolean) => {
    fetch(`/api/handle-state?isOpen=${checked ? 1 : 0}`, {
      method: 'GET',
      headers: {
        'Content-Type': 'application/json',
      },
    })
      .then(async (res) => {
        if (!res.ok) {
          setIsError(true);
        } else {
          const result = await res.json();
          setIsOpen(checked);
          if (checked === true) {
            setCurrentTimeUsage(result.execTime);
          }
          setIsError(false);
        }
      })
      .catch((err) => {
        console.log(err);
        setIsError(true);
      });
  };

  const updateTimeDifference = () => {
    const now = new Date();
    const targetTime = new Date(currentTimeUsage);
    const diff = Math.abs(now.getTime() - targetTime.getTime());

    const hours = Math.floor(diff / (1000 * 60 * 60));
    const minutes = Math.floor((diff % (1000 * 60 * 60)) / (1000 * 60));
    const seconds = Math.floor((diff % (1000 * 60)) / 1000);

    const formattedTime = `${String(hours).padStart(2, '0')}:${String(minutes).padStart(2, '0')}:${String(
      seconds
    ).padStart(2, '0')}`;

    if (currentTimeUsage !== 0) {
      setTimeDifference(formattedTime);
    }
  };

  useEffect(() => {
    setIsError(false);
    getLightState();
  }, []);

  useEffect(() => {
    const intervalId = setInterval(updateTimeDifference, 1000);
    return () => clearInterval(intervalId);
  }, [currentTimeUsage]);

  return (
    <div className={styles.control}>
      {isError ? (
        <NoticeBar
          closeable
          icon={<ExclamationTriangleOutline />}
          color='alert'
          content={'Network Error, failed to get the light state'}
          onClose={() => setIsError(false)}
        />
      ) : null}
      <div className={styles['operation-item']}>
        <p className={styles.title}>Turn on/off the light</p>
        <Switch
          checked={isOpen}
          onChange={(checked) => {
            handleState(checked);
          }}
          style={{
            '--checked-color': '#a03cf7',
            '--height': '36px',
            '--width': '60px',
          }}
        />
      </div>
      <div className={styles['operation-item']}>
        <p className={styles.title}>Current usage time</p>
        <p className={styles.content}>{isOpen ? timeDifference : 'N/A'}</p>
      </div>
    </div>
  );
};

export default Control;
