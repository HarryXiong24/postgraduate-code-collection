import type { NextApiRequest, NextApiResponse } from 'next';

type Response = {
  status: number;
  error?: string;
};

const handler = async (req: NextApiRequest, res: NextApiResponse<Response>) => {
  if (req.method === 'GET') {
    const isOpen = req.query.isOpen;

    if (isOpen === undefined) {
      res.status(400).json({ status: 400 });
    }

    const result = await fetch(`http://18.212.189.40:8080/handle-status?isOpen=${isOpen}`, {
      method: 'GET',
      headers: {
        'Content-Type': 'application/json',
      },
    });

    if (result.ok) {
      const data = await result.json();
      res.status(200).json(data);
    } else {
      res.status(500).json({ status: 500, error: 'Internal Server Error' });
    }
  }

  res.status(400).json({ status: 400, error: 'Bad Request' });
};

export default handler;
