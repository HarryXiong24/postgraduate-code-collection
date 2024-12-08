import { Button, Empty, Space } from 'antd-mobile';
import styles from './index.module.scss';
import { useEffect, useState } from 'react';
import ECharts from '@/components/echarts';
import { LeftOutline, RedoOutline, RightOutline } from 'antd-mobile-icons';

interface Data {
  date: string;
  duration: number;
}

const Analyze = () => {
  const [data, setData] = useState<Data[]>([]);
  const [page, setPage] = useState(1);

  const getTimeUsage = async (page: number) => {
    try {
      const res = await fetch(`/api/time-usage?page=${page}`, {
        method: 'GET',
        headers: {
          'Content-Type': 'application/json',
        },
      });
      if (res.ok) {
        const result = await res.json();
        setData(result?.data?.reverse() || []);
      }
    } catch (error) {
      console.log(error);
    }
  };

  useEffect(() => {
    getTimeUsage(1);
  }, []);

  return (
    <div className={styles.analyze}>
      <div className={styles.title}>Analyze your smart light</div>

      <div className={styles.mode}>
        <p className={styles.subtitle}>Time Pick Up</p>
        <div className={styles['operation-bar']}>
          <Button
            size='small'
            style={{ marginRight: 8 }}
            onClick={() => {
              setPage(page + 1);
              getTimeUsage(page + 1);
            }}
          >
            <Space>
              <LeftOutline />
            </Space>
          </Button>
          <Button
            disabled={page === 1}
            size='small'
            style={{ marginRight: 8 }}
            onClick={() => {
              if (page === 1) {
                return;
              }
              setPage(page - 1);
              getTimeUsage(page - 1);
            }}
          >
            <Space>
              <RightOutline />
            </Space>
          </Button>
          <Button
            size='small'
            onClick={() => {
              setPage(1);
              getTimeUsage(1);
            }}
          >
            <Space>
              <RedoOutline />
            </Space>
          </Button>
        </div>
      </div>

      <div className={styles.charts}>
        <p className={styles.subtitle}>Time Usage</p>
        {data.length ? (
          <ECharts
            style={{ height: '30rem', width: '100%' }}
            options={{
              title: {},
              tooltip: {
                trigger: 'axis',
                axisPointer: {
                  type: 'cross',
                  label: {
                    backgroundColor: '#6a7985',
                  },
                },
              },
              toolbox: {
                feature: {
                  mark: { show: true },
                  dataView: { show: true, readOnly: false },
                  magicType: { show: true, type: ['line', 'bar'] },
                  restore: {},
                  saveAsImage: {},
                },
              },
              grid: {
                left: '3%',
                right: '4%',
                bottom: '3%',
                containLabel: true,
              },
              xAxis: [
                {
                  type: 'category',
                  boundaryGap: false,
                  data: data.map((item) => item.date.slice(5)),
                },
              ],
              yAxis: [
                {
                  name: 'min',
                  type: 'value',
                  max: Math.max(...data.map((item) => Math.floor(item.duration / 60))) + 10,
                  min: 0,
                },
              ],
              series: [
                {
                  name: 'Time Usage(min)',
                  type: 'line',
                  stack: 'Total',
                  areaStyle: {},
                  emphasis: {
                    focus: 'series',
                  },
                  data: data.map((item) => Math.floor(item.duration / 60)),
                },
              ],
            }}
          />
        ) : (
          <Empty style={{ padding: '10rem 0' }} imageStyle={{ width: 128 }} description='本七天暂无数据' />
        )}
      </div>
    </div>
  );
};

export default Analyze;
