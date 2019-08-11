// CJiaohuDlg.cpp: 实现文件
//

#include "pch.h"
#include "GetFeature.h"
#include "CJiaohuDlg.h"
#include "afxdialogex.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CJiaohuDlg 对话框
IMPLEMENT_DYNAMIC(CJiaohuDlg, CDialogEx)
CJiaohuDlg::CJiaohuDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1_JIAOHU, pParent)
{
	
}

CJiaohuDlg::~CJiaohuDlg()
{
}

void CJiaohuDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_FEATURE, m_feature_folder_path);
	DDX_Control(pDX, IDC_LIST_VIDEOCLIP, m_listbox_videoclip);
	DDX_Control(pDX, IDC_SLIDER_SEEK, m_slider_seek);
	DDX_Control(pDX, IDC_PICTURE_PLAY, m_picture_static);
	DDX_Control(pDX, IDC_COMBO_BOX1, m_combobox1);
}


BEGIN_MESSAGE_MAP(CJiaohuDlg, CDialogEx)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CJiaohuDlg::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, &CJiaohuDlg::OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_PLAY_SLOWLY, &CJiaohuDlg::OnBnClickedButtonPlaySlowly)
	ON_BN_CLICKED(IDC_BUTTON_PLAY_FAST, &CJiaohuDlg::OnBnClickedButtonPlayFast)
	ON_BN_CLICKED(IDC_BUTTON_PLAY_FRAME, &CJiaohuDlg::OnBnClickedButtonPlayFrame)
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()
BOOL CJiaohuDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	//m_IsAccess_control_position = Access_control_position();

	// TODO:  在此添加额外的初始化
	/////////////////////////////////////////////////////////////////////////////
	//初始化滚动条变量
	m_slider_seek.SetPos(0);
	m_slider_seek.SetPageSize(10);//滚动条每次seek的间隔 //10秒
	m_combobox1.SetCurSel(0);
		//初始化全局变量 编解码库 SDL初始化等
	GetDlgItem(IDC_EDIT_TIMELENGTH)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_PROGRESS)->ShowWindow(SW_HIDE);
	//GetDlgItem(IDC_SLIDER_SEEK)->EnableWindow(FALSE);
	InitProgram();
	//////////////////////////////////////////////////////////////////////////
	InitVariable();
	get_control_original_proportion(); // 初始化窗口大小相关变量
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


int SplitString(LPCTSTR lpszStr, LPCTSTR lpszSplit, CStringArray& rArrString, BOOL bAllowNullString)
{
	rArrString.RemoveAll();
	CString szStr = lpszStr;
	szStr.TrimLeft();
	szStr.TrimRight();
	if (szStr.GetLength() == 0)
	{
		return 0;
	}
	CString szSplit = lpszSplit;
	if (szSplit.GetLength() == 0)
	{
		rArrString.Add(szStr);
		return 1;
	}
	CString s;
	int n;
	do {
		n = szStr.Find(szSplit);
		if (n > 0)
		{
			rArrString.Add(szStr.Left(n));
			szStr = szStr.Right(szStr.GetLength() - n - szSplit.GetLength());
			szStr.TrimLeft();
		}
		else if (n == 0)
		{
			if (bAllowNullString)
				rArrString.Add(_T(""));
			szStr = szStr.Right(szStr.GetLength() - szSplit.GetLength());
			szStr.TrimLeft();
		}
		else
		{
			if ((szStr.GetLength() > 0) || bAllowNullString)
				rArrString.Add(szStr);
			break;
		}
	} while (1);
	return rArrString.GetSize();
}

void CJiaohuDlg::OnBnClickedButtonOpen()
{
	// TODO: 在此添加控件通知处理程序代码
	
	//初始化变量等
	onstop();
	InitVariable();
	Sleep(500);
	//////////////////////////////////////////////////////////////////////////
	
	int h_hour = 0;//文件总时常显示的文本(时)
	int h_minute = 0;//文件总时常显示的文本(分)
	int h_second = 0;//文件总时常显示的文本（秒）
	// 文件扩展名过滤
	CString szFilter = _T("All Files (*.*)|*.*|avi Files (*.avi)|*.avi|rmvb Files (*.rmvb)|*.rmvb|3gp Files (*.3gp)|*.3gp|mp3 Files (*.mp3)|*.mp3|mp4 Files (*.mp4)|*.mp4|mpeg Files (*.ts)|*.ts|flv Files (*.flv)|*.flv|mov Files (*.mov)|*.mov||");
	CFileDialog dlg(TRUE, NULL, NULL, OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, szFilter, NULL);
	CString strFolderPath;
	CString time_display;
	if (IDOK == dlg.DoModal())
	{
		//建立视频数据源（加载视频文件）
		strFolderPath = dlg.GetFolderPath();
		CString Filename = dlg.GetPathName();
		m_feature_folder_path.SetWindowText(strFolderPath);
		USES_CONVERSION;
		m_sourceFile = W2A(Filename);

		//打开要被应用的输入文件
		m_streamstate->pFormatCtx = avformat_alloc_context();
		if (avformat_open_input(&m_streamstate->pFormatCtx, m_sourceFile, NULL, NULL) < 0)
		{
			MessageBox(_T("FFMPEG 打开文件失败 请确认文件类型和路径正确"), NULL, MB_OK);
			goto end;
		}

		//查看流信息
		if (avformat_find_stream_info(m_streamstate->pFormatCtx, NULL) < 0)
		{
			int msgboxID = MessageBox(_T("FFMPEG 获取文件信息失败 是否继续"), NULL, MB_YESNO);
			if (msgboxID != IDYES)
			{
				goto end;
			}
		}
		//输入信息错误写入
		av_dump_format(m_streamstate->pFormatCtx, 0, m_sourceFile, 0);



		//将文件信息填入列表框控件变量
		CString szSplit = _T("\\");
		CStringArray szList;
		int Count = SplitString(Filename, szSplit, szList, FALSE);
		CString a1 = szList.GetAt(Count-1);
		m_listbox_videoclip.AddString(a1);

		//得到视频信息的CONTEXT
		if (Open_codec_context(&m_video_stream_idx, m_streamstate->pFormatCtx, AVMEDIA_TYPE_VIDEO) >= 0)
		{
		//得到文件中时常
			

			m_streamstate->video_st = m_streamstate->pFormatCtx->streams[m_video_stream_idx];
			double video_time_base = av_q2d(m_streamstate->video_st->time_base);
			m_file_duration = m_streamstate->video_st->duration * video_time_base;
		//如果没有文件总时常
			if (m_file_duration < 0)
			{
				m_file_duration = 0;
			}
			m_video_dec_ctx = m_streamstate->video_st->codec;
			// Find the decoder for the video stream
			m_pvideo_codec = avcodec_find_decoder(m_video_dec_ctx->codec_id);
			if (m_pvideo_codec == NULL)
			{
				MessageBox(_T("Find video decoder失败 请重新加载"), NULL, MB_OK);
				goto end;
			}
			// Open codec
			if (avcodec_open2(m_video_dec_ctx, m_pvideo_codec,NULL) < 0)
			{
				MessageBox(_T("Could not open video codec 请重新加载"), NULL, MB_OK);
				goto end;
			}

			//视频的帧率
			m_dbFrameRate = av_q2d(m_streamstate->video_st->r_frame_rate);
			//这里说明是裸流
			if (m_dbFrameRate < 15 || m_dbFrameRate > 50)
			{
				//这种是MP3 有图片的
				//m_stream_type = 2;
				//m_dbFrameRate = 25.0;
			}
			//宽高,视频编码类型,视频yuv类型,spspps_buf,spspps_size
			m_dwWidth = m_streamstate->pFormatCtx->streams[m_video_stream_idx]->codec->width;
			m_dwHeight = m_streamstate->pFormatCtx->streams[m_video_stream_idx]->codec->height;
			m_video_codecID = m_streamstate->pFormatCtx->streams[m_video_stream_idx]->codec->codec_id;
			m_video_pixelfromat = m_streamstate->pFormatCtx->streams[m_video_stream_idx]->codec->pix_fmt;
			m_spspps_size = m_streamstate->pFormatCtx->streams[m_video_stream_idx]->codec->extradata_size;
			if (m_spspps_size < 1000)
			{
				memcpy(m_spspps, m_streamstate->pFormatCtx->streams[m_video_stream_idx]->codec->extradata, m_spspps_size);
			}
			//这里加一个判断是用于对不同的格式进行pts单位的统一化：统一到 “秒”,可扩展其它格式
			if (strstr(m_streamstate->pFormatCtx->iformat->name, "mpegts") != NULL)
			{
				double time_base = 90 * 1000;
				m_video_duration = time_base / m_dbFrameRate / 100000;
			}
			else
			{
				m_video_duration = 1 / m_dbFrameRate;
			}
		}

	}
	else
	{
		MessageBox(_T("获取文件名称失败 请重新加载"), NULL, MB_OK);
		goto end;
	}
	m_stream_type = 3;
	//////////////////////////////////////////////////////////////////////////
	//文件打开设置文件时长

	
	h_minute = (int)m_file_duration / 60;
	h_second = (int)m_file_duration % 60;
	h_hour = (int)h_minute / 60;
	h_minute = h_minute % 60;
	str.Format(_T("%.2d:%.2d:%.2d"), h_hour, h_minute, h_second);
	time_display.Format(_T("00:00:00/%s"), str);
	GetDlgItem(IDC_EDIT_TIMELENGTH)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDIT_TIMELENGTH)->SetWindowText(time_display);
	GetDlgItem(IDC_SLIDER_SEEK)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_PLAY)->SetWindowText(_T("停止"));
	m_slider_seek.SetPos(0);
	m_slider_seek.SetRange(0, m_file_duration, 0);
	m_streamstate->read_tid = SDL_CreateThread(read_thread, (void*)this);
	if (!m_streamstate->read_tid)
	{
		MessageBox(_T("创建读取线程失败 清除重新创建"), NULL, MB_OK);
		goto end;
	}
end:
	{
	//////////////////////////////////////////////////////////////////////////
	//这里可以做一些其他工作
	//////////////////////////////////////////////////////////////////////////
	}
}

void CJiaohuDlg::video_refresh(void* opaque)
{
	StreamState* streamstate = (StreamState*)opaque;
	if (streamstate == NULL)
	{
		return;
	}
	VideoPicture* vp;
	double delay;

	//////////////////////////////////////////////////////////////////////////
		//设置当前时间显示
	
	int h_hour = 0;
	int h_minute = 0;
	int h_second = 0;
	CString time_display;
	//如果只有视频
	if (m_stream_type == 3)
	{
		Sleep(10);

		h_minute = (int)m_streamstate->video_clock / 60;
		h_second = (int)m_streamstate->video_clock % 60;
		h_hour = (int)h_minute / 60;
		h_minute = h_minute % 60;
		curr.Format(_T("%.2d:%.2d:%.2d"), h_hour, h_minute, h_second);
		if (m_Isstop) //如果已经停止
		{
			GetDlgItem(IDC_EDIT_TIMELENGTH)->ShowWindow(SW_HIDE);
			//进度条随着时间滚动
			m_slider_seek.SetPos(0);
		}
		else
		{
			
			time_display.Format(_T("%s/%s"), curr,str);
			GetDlgItem(IDC_EDIT_TIMELENGTH)->SetWindowText(time_display);
			//进度条随着时间滚动
			if (m_pause_play)
			{
				m_slider_seek.SetPos(m_streamstate->video_clock);
			}
		}
	}

	if (streamstate->video_st)
	{
		if (streamstate->pictq_size == 0)
		{
			Sleep(1);
		}
		else
		{
			//取出要显示视频的图像数据
			vp = &streamstate->pictq[streamstate->pictq_rindex];
			//算出一帧视频本应该显示的时间
			delay = m_video_duration;
			if (normal)
			{
				Sleep(delay * 1100);	
			}
			else if (fast) {
				Sleep(delay * 500);
			}
			else if (slow) {
				Sleep(delay * 2000);
			}
			
			/* update queue for next picture! */
			if (++streamstate->pictq_rindex == VIDEO_PICTURE_QUEUE_SIZE)
			{
				streamstate->pictq_rindex = 0;
			}
			SDL_LockMutex(streamstate->pictq_mutex);
			streamstate->pictq_size--;
			SDL_CondSignal(streamstate->pictq_cond);
			SDL_UnlockMutex(streamstate->pictq_mutex);

			/* show the picture! */
			video_display(streamstate);
		}
	}


}
void CJiaohuDlg::video_display(StreamState* m_streamstate)
{
	SDL_Rect rect;
	VideoPicture* vp;

	if (!m_screen)
	{
		video_open(m_streamstate);
	}
	vp = &m_streamstate->pictq[m_streamstate->pictq_rindex];
	if (vp->bmp)
	{
		if (vp->bmp->w != m_screen_width || vp->bmp->h != m_screen_height)
		{
			//这里一定要注意 当sdl的窗口大小改变的时候 切换那一瞬间 
			//m_streamstate->pictq[m_streamstate->pictq_rindex] 这里的数据有可能还是原来窗口大小的数据 
			//这时将这一真不花到sdl窗口上防止窗口死掉。也可以用get_next_frame控制
			return;
		}
		rect.x = 0;
		rect.y = 0;
		rect.w = m_screen_width;
		rect.h = m_screen_height;
		SDL_DisplayYUVOverlay(vp->bmp, &rect);
	}
}
int refresh_thread(LPVOID lpParam)
{
	CJiaohuDlg* pDlg = (CJiaohuDlg*)lpParam;
	ASSERT(pDlg);

	while (!pDlg->m_Isstop)
	{
		//添加刷新函数
		if (pDlg->m_pause_play) {
			pDlg->video_refresh(pDlg->m_streamstate);
		}
		
		//FIXME ideally we should wait the correct time but SDLs event passing m_streamstate so slow it would be silly
	}
	return 0;
}
int video_thread(void* arg)
{
	CJiaohuDlg* pDlg = (CJiaohuDlg*)arg;
	ASSERT(pDlg);

	//AVPacket pkt1, *packet = &pkt1;
	int frameFinished;
	AVFrame* pFrame = NULL;

	pFrame = avcodec_alloc_frame();

	for (;;)
	{
		AVPacket pkt;
		if (pDlg->m_Isstop)
		{
			break;
		}
		if (pDlg->m_pause_play) //如果正在播放
		{
			if (pDlg->packet_queue_get(&pDlg->m_streamstate->videoq, &pkt, VIDEO_ID) < 0)
			{
				// means we quit getting packets
				break;
			}
			if (pkt.data == pDlg->m_flush_pkt.data)
			{
				avcodec_flush_buffers(pDlg->m_streamstate->video_st->codec);
				continue;
			}
			// Decode video frame
			avcodec_decode_video2(pDlg->m_streamstate->video_st->codec, pFrame, &frameFinished, &pkt);
			av_free_packet(&pkt);

			// Did we get a video frame?
			if (frameFinished)
			{
				pDlg->m_streamstate->video_clock += pDlg->m_video_duration;
				if (pDlg->queue_picture(pDlg->m_streamstate, pFrame, pDlg->m_streamstate->video_clock) < 0)
				{
					break;
				}
			}
		}
		else
		{
			continue;
		}
	}
	avcodec_free_frame(&pFrame);
	return 0;
}
int CJiaohuDlg::queue_picture(StreamState* m_streamstate, AVFrame* pFrame, double pts)
{
	VideoPicture* vp = NULL;
	AVFrame* pOutputFrame = NULL;
	uint8_t* pOutput_buf = NULL;
	int Out_size = 0;
	struct SwsContext* img_convert_ctx;

	pOutputFrame = avcodec_alloc_frame();

	/* wait until we have space for a new pic */
	SDL_LockMutex(m_streamstate->pictq_mutex);
	while (m_streamstate->pictq_size >= VIDEO_PICTURE_QUEUE_SIZE)
	{
		SDL_CondWait(m_streamstate->pictq_cond, m_streamstate->pictq_mutex);
	}
	SDL_UnlockMutex(m_streamstate->pictq_mutex);

	// windex m_streamstate set to 0 initially
	vp = &m_streamstate->pictq[m_streamstate->pictq_windex];

	/* allocate or resize the buffer! */
	if (!vp->bmp || vp->width != m_screen_width || vp->height != m_screen_height)
	{
		SDL_Event event;

		vp->allocated = 0;
		/* we have to do it in the main thread */
		alloc_picture(m_streamstate);
		/* wait until we have a picture allocated */
		SDL_LockMutex(m_streamstate->pictq_mutex);
		while (!vp->allocated)
		{
			SDL_CondWait(m_streamstate->pictq_cond, m_streamstate->pictq_mutex);
		}
		/* if the queue m_streamstate aborted, we have to pop the pending ALLOC event or wait for the allocation to complete */
		//if (SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_EVENTMASK(FF_ALLOC_EVENT)) != 1)
		//{
		//	while (!vp->allocated) 
		//	{
		//		SDL_CondWait(m_streamstate->pictq_cond, m_streamstate->pictq_mutex);
		//	}
		//}
		SDL_UnlockMutex(m_streamstate->pictq_mutex);
	}
	/* We have a place to put our picture on the queue */

	if (vp->bmp)
	{

		SDL_LockYUVOverlay(vp->bmp);
		/* point pict at the queue */

		pOutputFrame->data[0] = vp->bmp->pixels[0];
		pOutputFrame->data[1] = vp->bmp->pixels[2];
		pOutputFrame->data[2] = vp->bmp->pixels[1];

		pOutputFrame->linesize[0] = vp->bmp->pitches[0];
		pOutputFrame->linesize[1] = vp->bmp->pitches[2];
		pOutputFrame->linesize[2] = vp->bmp->pitches[1];

		img_convert_ctx = sws_getContext(pFrame->width, pFrame->height, m_streamstate->video_st->codec->pix_fmt,
			m_screen_width, m_screen_height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
		if (img_convert_ctx == NULL)
		{
			fprintf(stderr, "Cannot initialize the conversion context!\n");
			getchar();
		}

		// 将图片转换为RGB格式
		sws_scale(img_convert_ctx, pFrame->data, pFrame->linesize, 0,
			pFrame->height, pOutputFrame->data, pOutputFrame->linesize);

		SDL_UnlockYUVOverlay(vp->bmp);
		vp->pts = pts;
		/* now we inform our display thread that we have a pic ready */
		if (++m_streamstate->pictq_windex == VIDEO_PICTURE_QUEUE_SIZE)
		{
			m_streamstate->pictq_windex = 0;
		}
		SDL_LockMutex(m_streamstate->pictq_mutex);
		m_streamstate->pictq_size++;
		SDL_UnlockMutex(m_streamstate->pictq_mutex);
	}

	sws_freeContext(img_convert_ctx);

	return 0;
}

int CJiaohuDlg::video_open(StreamState* m_streamstate)
{
	int flags = SDL_HWSURFACE | SDL_ASYNCBLIT | SDL_HWACCEL;
	int w, h;
	VideoPicture* vp = &m_streamstate->pictq[m_streamstate->pictq_rindex];

	////这里是获取到 STATIC控件的大小 
	////这里做判断是因为 如果改变大小时 的 宽 和高 出现 奇数则图像会不正确 或 中断 要偶数
	if (m_is_full_screen)
	{
		flags |= SDL_FULLSCREEN;
	}
	else
	{
		flags |= SDL_RESIZABLE;
	}

	if (m_is_full_screen && m_fs_screen_width)
	{
		w = m_fs_screen_width;
		h = m_fs_screen_height;
	}
	else
	{
		//要改变的大小
		w = m_screen_width;
		h = m_screen_height;
	}

	if (m_screen && m_screen_width == m_screen->w && m_screen->w == w
		&& m_screen_height == m_screen->h && m_screen->h == h)
	{
		return 0;
	}

	m_screen = SDL_SetVideoMode(w, h, 0, flags);

	//SDL刷新 否则最大化然后缩小有问题 
	SDL_Flip(m_screen);
	//这里一定要重新刷新MFC窗口
	Invalidate(TRUE);
	if (!m_screen)
	{
		fprintf(stderr, "SDL: could not set video mode - exiting\n");
		onstop();
	}
	return 0;
}


void CJiaohuDlg::alloc_picture(void* userdata)
{
	StreamState* m_streamstate = (StreamState*)userdata;
	VideoPicture* vp;

	vp = &m_streamstate->pictq[m_streamstate->pictq_windex];
	if (vp->bmp)
	{
		// we already have one make another, bigger/smaller
		SDL_FreeYUVOverlay(vp->bmp);
	}

	video_open(m_streamstate);
	// Allocate a place to put our YUV image on that screen
	vp->bmp = SDL_CreateYUVOverlay(m_screen_width,
		m_screen_height,
		SDL_YV12_OVERLAY,
		m_screen);
	vp->width = m_screen_width;
	vp->height = m_screen_height;

	SDL_LockMutex(m_streamstate->pictq_mutex);
	vp->allocated = 1;
	SDL_CondSignal(m_streamstate->pictq_cond);
	SDL_UnlockMutex(m_streamstate->pictq_mutex);
}
int CJiaohuDlg::stream_component_open(StreamState* m_streamstate, int stream_index)
{
	AVFormatContext* ic = m_streamstate->pFormatCtx;
	SDL_AudioSpec wanted_spec, spec;

	if (stream_index < 0 || stream_index >= ic->nb_streams)
	{
		return -1;
	}

	ic->streams[stream_index]->discard = AVDISCARD_DEFAULT;

	
	if (!m_pvideo_codec || avcodec_open2(m_video_dec_ctx, m_pvideo_codec, NULL) < 0)
	{
		return -1;
	}
	m_streamstate->video_st = ic->streams[stream_index];
	packet_queue_init(&m_streamstate->videoq);
	packet_queue_start(&m_streamstate->videoq);
	m_streamstate->video_tid = SDL_CreateThread(video_thread, this);
	
	return 1;
}

int CJiaohuDlg::stream_component_close(StreamState* m_streamstate, int stream_index)
{
	AVFormatContext* ic = m_streamstate->pFormatCtx;

	if (stream_index < 0 || stream_index >= ic->nb_streams)
	{
		return -1;
	}

	ic->streams[stream_index]->discard = AVDISCARD_ALL;


		packet_queue_abort(&m_streamstate->videoq);
		/* note: we also signal this mutex to make sure we deblock the
		   video thread in all cases */
		SDL_LockMutex(m_streamstate->pictq_mutex);
		SDL_CondSignal(m_streamstate->pictq_cond);
		SDL_UnlockMutex(m_streamstate->pictq_mutex);
		//SDL_WaitThread(m_streamstate->video_tid,NULL);
		SDL_KillThread(m_streamstate->video_tid);
		packet_queue_flush(&m_streamstate->videoq);
		m_streamstate->video_st = NULL;
		avcodec_close(m_video_dec_ctx);
	
	return 1;
}

void CJiaohuDlg::packet_queue_init(PacketQueue* q)
{
	memset(q, 0, sizeof(PacketQueue));
	q->mutex = SDL_CreateMutex();
	q->cond = SDL_CreateCond();
}

int CJiaohuDlg::packet_queue_put(PacketQueue* q, AVPacket* pkt)
{
	int ret;

	/* duplicate the packet */
	if (pkt != &m_flush_pkt && av_dup_packet(pkt) < 0)
	{
		return -1;
	}

	SDL_LockMutex(q->mutex);
	ret = packet_queue_put_private(q, pkt);
	SDL_UnlockMutex(q->mutex);

	if (pkt != &m_flush_pkt && ret < 0)
	{
		av_free_packet(pkt);
	}

	return ret;
}

void CJiaohuDlg::packet_queue_start(PacketQueue* q)
{
	SDL_LockMutex(q->mutex);
	packet_queue_put_private(q, &m_flush_pkt);
	SDL_UnlockMutex(q->mutex);
}

int CJiaohuDlg::packet_queue_get(PacketQueue* q, AVPacket* pkt, int queue_type)
{
	AVPacketList* pkt1;
	int ret;

	SDL_LockMutex(q->mutex);

	for (;;)
	{
		pkt1 = q->first_pkt;
		if (pkt1)
		{
			q->first_pkt = pkt1->next;
			if (!q->first_pkt)
			{
				q->last_pkt = NULL;
			}
			q->nb_packets--;
			//q->size -= pkt1->pkt.size;
			q->size -= pkt1->pkt.size;
			*pkt = pkt1->pkt;
			av_free(pkt1);
			ret = 1;
			break;
		}
		else
		{
			int sdlcondwait = SDL_CondWait(q->cond, q->mutex);
		}
	}
	SDL_UnlockMutex(q->mutex);
	return ret;
}

void CJiaohuDlg::packet_queue_abort(PacketQueue* q)
{
	SDL_LockMutex(q->mutex);
	SDL_CondSignal(q->cond);
	SDL_UnlockMutex(q->mutex);
}
void CJiaohuDlg::packet_queue_flush(PacketQueue* q)
{
	AVPacketList* pkt, * pkt1;

	SDL_LockMutex(q->mutex);
	for (pkt = q->first_pkt; pkt != NULL; pkt = pkt1)
	{
		pkt1 = pkt->next;
		av_free_packet(&pkt->pkt);
		av_freep(&pkt);
	}
	q->last_pkt = NULL;
	q->first_pkt = NULL;
	q->nb_packets = 0;
	q->size = 0;
	SDL_UnlockMutex(q->mutex);
}

void CJiaohuDlg::packet_queue_destroy(PacketQueue* q)
{
	packet_queue_flush(q);
	SDL_DestroyMutex(q->mutex);
	SDL_DestroyCond(q->cond);
}
int CJiaohuDlg::packet_queue_put_private(PacketQueue* q, AVPacket* pkt)
{
	AVPacketList* pkt1;

	pkt1 = (AVPacketList*)av_malloc(sizeof(AVPacketList));
	if (!pkt1)
	{
		return -1;
	}
	pkt1->pkt = *pkt;
	pkt1->next = NULL;

	if (!q->last_pkt)
	{
		q->first_pkt = pkt1;
	}
	else
	{
		q->last_pkt->next = pkt1;
	}
	q->last_pkt = pkt1;
	q->nb_packets++;
	//q->size += pkt1->pkt.size + sizeof(*pkt1);
	q->size += pkt1->pkt.size;
	/* XXX: should duplicate packet data in DV case */
	SDL_CondSignal(q->cond);
	return 0;
}
int read_thread(LPVOID lpParam)
{
	CJiaohuDlg* pDlg = (CJiaohuDlg*)lpParam;
	ASSERT(pDlg);

	int ret = -1;
	int error = -1;
	int eof = 0;
	AVPacket pkt1, * pkt = &pkt1;

	//创建刷新线程
	pDlg->m_streamstate->refresh_tid = SDL_CreateThread(refresh_thread, pDlg);

	/* open the streams */

	if (AVMEDIA_TYPE_VIDEO >= 0)
	{
		ret = pDlg->stream_component_open(pDlg->m_streamstate, AVMEDIA_TYPE_VIDEO);
	}

	if (pDlg->m_video_stream_idx < 0 )
	{
		ret = -1;
		goto fail;
	}

	for (;;)
	{
		if (pDlg->m_Isstop)
		{
			break;
		}
		// seek stuff goes here
		if (pDlg->m_streamstate->seek_req)
		{
			int stream_index = -1;
			//转化成纳秒
			int64_t seek_target = pDlg->m_streamstate->seek_pos * 1000000;

			//如果不是只有音频
			if (pDlg->m_stream_type != 2)
			{
				if (pDlg->m_video_stream_idx >= 0)
				{
					stream_index = pDlg->m_video_stream_idx;
				}

			}
			else
			{
				if (pDlg->m_video_stream_idx >= 0)
				{
					stream_index = pDlg->m_video_stream_idx;
				}
			}
			if (stream_index >= 0)
			{
				AVRational time_base_q;
				time_base_q.num = 1;
				time_base_q.den = AV_TIME_BASE;
				//这里一定要注意：不单纯的是从秒转成毫秒，//seek_target = seek_target / 1000; 这样做是不对的
				seek_target = av_rescale_q(seek_target, time_base_q, pDlg->m_streamstate->pFormatCtx->streams[stream_index]->time_base);
			}
			error = av_seek_frame(pDlg->m_streamstate->pFormatCtx, stream_index, seek_target, pDlg->m_streamstate->seek_flags);
			//seek成功
			if (error >= 0)
			{
				if (pDlg->m_video_stream_idx >= 0)
				{
					pDlg->packet_queue_flush(&pDlg->m_streamstate->videoq);
					pDlg->packet_queue_put(&pDlg->m_streamstate->videoq, &pDlg->m_flush_pkt);
				}
				
				//只有视频
				 if (pDlg->m_stream_type == 3)
				{
					pDlg->m_streamstate->video_clock = ((int)(pDlg->m_streamstate->seek_pos / pDlg->m_video_duration) + 1) * pDlg->m_video_duration;
				}
			}
			pDlg->m_streamstate->seek_req = 0;
			pDlg->m_streamstate->seek_time = 0;
			eof = 0;
		}

		/* if the queue are full, no need to read more */
		if (pDlg->m_streamstate->videoq.size > MAX_QUEUE_SIZE
			|| (pDlg->m_streamstate->videoq.nb_packets > MIN_FRAMES))
		{
			/* wait 10 ms */
			SDL_Delay(10);
			continue;
		}

		if (eof)
		{
			//到文件末尾 放入队列一个空包
			if (pDlg->m_video_stream_idx >= 0)
			{
				av_init_packet(pkt);
				pkt->data = NULL;
				pkt->size = 0;
				pkt->stream_index = pDlg->m_video_stream_idx;
				pDlg->packet_queue_put(&pDlg->m_streamstate->videoq, pkt);
			}

			SDL_Delay(10);
			if ( pDlg->m_streamstate->videoq.size == 0)
			{
				ret = AVERROR_EOF;
				goto fail;
			}
			eof = 0;
			continue;
		}
		//这里确定文件到了末尾
		ret = av_read_frame(pDlg->m_streamstate->pFormatCtx, pkt);
		if (ret < 0)
		{
			if (ret == AVERROR_EOF || url_feof(pDlg->m_streamstate->pFormatCtx->pb))
			{
				eof = 1;
			}
			if (pDlg->m_streamstate->pFormatCtx->pb && pDlg->m_streamstate->pFormatCtx->pb->error)
			{
				break;
			}
			SDL_Delay(100); /* wait for user event */
			continue;
		}
		// Is this a packet from the video stream?
		if (pkt->stream_index == pDlg->m_video_stream_idx)
		{
			pDlg->packet_queue_put(&pDlg->m_streamstate->videoq, pkt);
		}
		else
		{
			av_free_packet(pkt);
		}
	}
	ret = 0;

fail:
	/* close each stream */
	VideoPicture* vp;
	int i;
	pDlg->m_Isstop = 1;


	if (pDlg->m_video_stream_idx >= 0)
	{
		pDlg->stream_component_close(pDlg->m_streamstate, pDlg->m_video_stream_idx);
	}
	if (pDlg->m_streamstate->pFormatCtx)
	{
		avformat_close_input(&pDlg->m_streamstate->pFormatCtx);
	}
	if (ret == AVERROR_EOF)
	{
		//退出操作
//////////////////////////////////////////////////////////////////////////
		if (pDlg->m_streamstate->refresh_tid)
		{
			//SDL_WaitThread(pDlg->m_streamstate->refresh_tid,NULL);
			SDL_KillThread(pDlg->m_streamstate->refresh_tid);
		}
		if (pDlg->m_streamstate->videoq.mutex)
		{
			pDlg->packet_queue_destroy(&pDlg->m_streamstate->videoq);
		}
		/* free all pictures */
		for (i = 0; i < VIDEO_PICTURE_QUEUE_SIZE; i++)
		{
			vp = &pDlg->m_streamstate->pictq[i];
			if (vp->bmp)
			{
				SDL_FreeYUVOverlay(vp->bmp);
				vp->bmp = NULL;
			}
		}
		//////////////////////////////////////////////////////////////////////////
		pDlg->m_streamstate->pFormatCtx = NULL;
		pDlg->m_streamstate->video_st = NULL;
		pDlg->m_streamstate->videoq.first_pkt = NULL;
		pDlg->m_streamstate->videoq.last_pkt = NULL;
		pDlg->m_streamstate->videoq.nb_packets = 0;
		pDlg->m_streamstate->videoq.size = 0;
		pDlg->m_streamstate->videoq.mutex = NULL;
		pDlg->m_streamstate->videoq.cond = NULL;
		//刷新packet
		if (pDlg->m_flush_pkt.data)
		{
			av_free_packet(&pDlg->m_flush_pkt);
		}
		pDlg->m_streamstate->pictq[0].bmp = NULL;
		pDlg->m_streamstate->pictq[0].width = 0;
		pDlg->m_streamstate->pictq[0].height = 0;
		pDlg->m_streamstate->pictq[0].allocated = 0;
		pDlg->m_streamstate->pictq[0].pts = 0.0;
		pDlg->m_streamstate->pictq_size = 0;
		pDlg->m_streamstate->pictq_rindex = 0;
		pDlg->m_streamstate->pictq_windex = 0;
		//释放互斥变量 条件变量
		if (pDlg->m_streamstate->pictq_mutex)
		{
			SDL_DestroyMutex(pDlg->m_streamstate->pictq_mutex);
		}
		if (pDlg->m_streamstate->pictq_cond)
		{
			SDL_DestroyCond(pDlg->m_streamstate->pictq_cond);
		}
		pDlg->m_streamstate->pictq_mutex = NULL;
		pDlg->m_streamstate->pictq_cond = NULL;
		pDlg->m_streamstate->read_tid = NULL;
		pDlg->m_streamstate->video_tid = NULL;
		pDlg->m_streamstate->refresh_tid = NULL;
		pDlg->m_streamstate->video_clock = 0.0;
		pDlg->m_streamstate->seek_time = 0.0;
		pDlg->m_streamstate->seek_req = 0;
		pDlg->m_streamstate->seek_flags = 0;
		pDlg->m_streamstate->seek_pos = 0;

		//////////////////////////////////////////////////////////////////////////
		//pDlg->m_fs_screen_width;
		//pDlg->m_fs_screen_height; 
		//AVPacket m_flush_pkt;
		
		pDlg->m_sourceFile = NULL;
		pDlg->m_file_duration = 0.0;
		pDlg->m_video_dec_ctx = NULL;
		pDlg->m_pvideo_codec = NULL;
		//释放窗口 
		if (pDlg->m_screen)
		{
			SDL_FreeSurface(pDlg->m_screen);
			pDlg->m_screen = NULL;
		}
		pDlg->m_is_full_screen = 0;
		pDlg->m_screen_width = 0;
		pDlg->m_screen_height = 0;
		pDlg->m_Isstop = 0;
		pDlg->m_pause_play = 0;
		pDlg->m_slider_pos = 0;
		pDlg->m_stream_type = 0;

		//////////////////////////////////////////////////////////////////////////
		//video pram
		pDlg->m_video_stream_idx = 0;
		pDlg->m_dbFrameRate = 0.0;
		pDlg->m_video_duration = 0.0;
		pDlg->m_dwWidth = 0;
		pDlg->m_dwHeight = 0;
		pDlg->m_video_codecID = AV_CODEC_ID_NONE;
		pDlg->m_video_pixelfromat = AV_PIX_FMT_NONE;
		memset(pDlg->m_spspps, 0, 100);
		pDlg->m_spspps_size = 0;


		//////////////////////////////////////////////////////////////////////////
		pDlg->m_Picture_rect.left = 0;
		pDlg->m_Picture_rect.right = 0;
		pDlg->m_Picture_rect.top = 0;
		pDlg->m_Picture_rect.bottom = 0;
		//////////////////////////////////////////////////////////////////////////


		//清空列表框中所有的数据
		//pDlg->m_listbox_videoclip.ResetContent();

		//将滚动条设置到最前端
		pDlg->m_slider_seek.SetPos(0);

		//初始化CEDIT 时间控件
		//pDlg->GetDlgItem(IDC_EDIT_TIMELENGTH)->ShowWindow(SW_HIDE);

		pDlg->Invalidate(TRUE);
	}
	return 1;
}
void CJiaohuDlg::onstop()
{
	m_Isstop = 1;
	Sleep(500);
	//销毁变量等
	UinitVariable();
	//将滚动条设置到最前端
	m_slider_seek.SetPos(0);
	Invalidate(TRUE);
}

int CJiaohuDlg::InitProgram()
{
	int flags;
	/* register all codecs, demux and protocols */
	av_register_all();
	avcodec_register_all();
	avformat_network_init();

	//将CSTATIC控件和sdl显示窗口关联 
	HWND hWnd = this->GetDlgItem(IDC_PICTURE_PLAY)->GetSafeHwnd();
	if (hWnd != NULL)
	{
		char sdl_var[64];
		sprintf(sdl_var, "SDL_WINDOWID=%d", hWnd);    //主窗口句柄      //这里一定不能有空格SDL_WINDOWID=%d"
		SDL_putenv(sdl_var);
		char* myvalue = SDL_getenv("SDL_WINDOWID");   //让SDL取得窗口ID  
	}

	//SDL初始化
	flags = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER;
	int  sdlinit = SDL_Init(flags);
	if (sdlinit)
	{
		char* sss = SDL_GetError();
		fprintf(stderr, "Could not initialize SDL - %s\n", SDL_GetError());
		fprintf(stderr, "(Did you set the DISPLAY variable?)\n");
		return -1;
	}

	//设置SDL事件状态
	SDL_EventState(SDL_ACTIVEEVENT, SDL_IGNORE);
	SDL_EventState(SDL_SYSWMEVENT, SDL_IGNORE);
	SDL_EventState(SDL_USEREVENT, SDL_IGNORE);

	//////////////////////////////////////////////////////////////////////////
		//音视频全局结构体初始化 这里已经清零
	m_streamstate = NULL;
	m_streamstate = (StreamState*)calloc(1, sizeof(StreamState));
	if (!m_streamstate)
	{
		return -1;
	}
	m_streamstate->pFormatCtx = NULL;
	m_streamstate->video_st = NULL;
	m_streamstate->videoq.first_pkt = NULL;
	m_streamstate->videoq.last_pkt = NULL;
	m_streamstate->videoq.nb_packets = 0;
	m_streamstate->videoq.size = 0;
	m_streamstate->videoq.mutex = NULL;
	m_streamstate->videoq.cond = NULL;
	m_streamstate->pictq[0].bmp = NULL;
	m_streamstate->pictq[0].width = 0;
	m_streamstate->pictq[0].height = 0;
	m_streamstate->pictq[0].allocated = 0;
	m_streamstate->pictq[0].pts = 0.0;
	m_streamstate->pictq_size = 0;
	m_streamstate->pictq_rindex = 0;
	m_streamstate->pictq_windex = 0;
	m_streamstate->pictq_mutex = NULL;
	m_streamstate->pictq_cond = NULL;
	m_streamstate->read_tid = NULL;
	m_streamstate->video_tid = NULL;
	m_streamstate->refresh_tid = NULL;
	m_streamstate->video_clock = 0.0;
	m_streamstate->seek_time = 0.0;
	m_streamstate->seek_req = 0;
	m_streamstate->seek_flags = 0;
	m_streamstate->seek_pos = 0;

	//////////////////////////////////////////////////////////////////////////
	//获取全屏的屏幕宽高  这里只能调用一次 第二次调用时 大小将改变
	const SDL_VideoInfo* vi = SDL_GetVideoInfo();
	m_fs_screen_width = vi->current_w;
	m_fs_screen_height = vi->current_h;
	//AVPacket m_flush_pkt;
	m_sourceFile = NULL;
	m_file_duration = 0.0;
	m_video_dec_ctx = NULL;
	m_pvideo_codec = NULL;
	m_screen = NULL;
	m_is_full_screen = 0;
	m_screen_width = 0;
	m_screen_height = 0;
	m_Isstop = 0;
	m_pause_play = 0;
	m_slider_pos = 0;
	m_stream_type = 0;

	//////////////////////////////////////////////////////////////////////////
	//video pram
	m_video_stream_idx = 0;
	m_dbFrameRate = 0.0;
	m_video_duration = 0.0;
	m_dwWidth = 0;
	m_dwHeight = 0;
	m_video_codecID = AV_CODEC_ID_NONE;
	m_video_pixelfromat = AV_PIX_FMT_NONE;
	memset(m_spspps, 0, 100);
	m_spspps_size = 0;

	m_Picture_rect.left = 0;
	m_Picture_rect.right = 0;
	m_Picture_rect.top = 0;
	m_Picture_rect.bottom = 0;
	//////////////////////////////////////////////////////////////////////////
	return 1;
}
int CJiaohuDlg::UinitProgram()
{
	//////////////////////////////////////////////////////////////////////////
		//释放全局结构体
	m_streamstate->pFormatCtx = NULL;

	m_streamstate->video_st = NULL;
	m_streamstate->videoq.first_pkt = NULL;
	m_streamstate->videoq.last_pkt = NULL;
	m_streamstate->videoq.nb_packets = 0;
	m_streamstate->videoq.size = 0;
	m_streamstate->videoq.mutex = NULL;
	m_streamstate->videoq.cond = NULL;
	m_streamstate->pictq[0].bmp = NULL;
	m_streamstate->pictq[0].width = 0;
	m_streamstate->pictq[0].height = 0;
	m_streamstate->pictq[0].allocated = 0;
	m_streamstate->pictq[0].pts = 0.0;
	m_streamstate->pictq_size = 0;
	m_streamstate->pictq_rindex = 0;
	m_streamstate->pictq_windex = 0;
	m_streamstate->pictq_mutex = NULL;
	m_streamstate->pictq_cond = NULL;
	m_streamstate->read_tid = NULL;
	m_streamstate->video_tid = NULL;
	m_streamstate->refresh_tid = NULL;
	m_streamstate->video_clock = 0.0;
	m_streamstate->seek_time = 0.0;
	m_streamstate->seek_req = 0;
	m_streamstate->seek_flags = 0;
	m_streamstate->seek_pos = 0;
	free(m_streamstate);
	m_streamstate = NULL;

	//////////////////////////////////////////////////////////////////////////
	m_fs_screen_width = 0;
	m_fs_screen_height = 0;
	//AVPacket m_flush_pkt;
	m_sourceFile =NULL;
	m_file_duration = 0.0;
	m_video_dec_ctx = NULL;
	m_pvideo_codec = NULL;
	m_screen = NULL;
	m_is_full_screen = 0;
	m_screen_width = 0;
	m_screen_height = 0;
	m_Isstop = 0;
	m_pause_play = 0;
	m_slider_pos = 0;
	m_stream_type = 0;

	//////////////////////////////////////////////////////////////////////////
	//video pram
	m_video_stream_idx = 0;
	m_dbFrameRate = 0.0;
	m_video_duration = 0.0;
	m_dwWidth = 0;
	m_dwHeight = 0;
	m_video_codecID = AV_CODEC_ID_NONE;
	m_video_pixelfromat = AV_PIX_FMT_NONE;
	memset(m_spspps, 0, 100);
	m_spspps_size = 0;

	//////////////////////////////////////////////////////////////////////////
	m_Picture_rect.left = 0;
	m_Picture_rect.right = 0;
	m_Picture_rect.top = 0;
	m_Picture_rect.bottom = 0;
	//////////////////////////////////////////////////////////////////////////
	avformat_network_deinit();
	SDL_Quit();
	return 1;
}
int CJiaohuDlg::InitVariable()
{
	//////////////////////////////////////////////////////////////////////////
		//音视频全局结构体初始化
	m_streamstate->pFormatCtx = NULL;
	m_streamstate->video_st = NULL;
	m_streamstate->videoq.first_pkt = NULL;
	m_streamstate->videoq.last_pkt = NULL;
	m_streamstate->videoq.nb_packets = 0;
	m_streamstate->videoq.size = 0;
	m_streamstate->videoq.mutex = NULL;
	m_streamstate->videoq.cond = NULL;
	m_streamstate->pictq[0].bmp = NULL;
	m_streamstate->pictq[0].width = 0;
	m_streamstate->pictq[0].height = 0;
	m_streamstate->pictq[0].allocated = 0;
	m_streamstate->pictq[0].pts = 0.0;
	m_streamstate->pictq_size = 0;
	m_streamstate->pictq_rindex = 0;
	m_streamstate->pictq_windex = 0;
	m_streamstate->pictq_mutex = NULL;
	m_streamstate->pictq_cond = NULL;
	//创建全局条件变量互斥变量
	m_streamstate->pictq_mutex = SDL_CreateMutex();
	m_streamstate->pictq_cond = SDL_CreateCond();
	m_streamstate->read_tid = NULL;
	m_streamstate->video_tid = NULL;
	m_streamstate->refresh_tid = NULL;
	m_streamstate->video_clock = 0.0;
	m_streamstate->seek_time = 0.0;
	m_streamstate->seek_req = 0;
	m_streamstate->seek_flags = 0;
	m_streamstate->seek_pos = 0;

	//////////////////////////////////////////////////////////////////////////
	//m_fs_screen_width
	//m_fs_screen_height
	//刷新packet初始化
	av_init_packet(&m_flush_pkt);
	m_flush_pkt.data = (uint8_t*)(intptr_t)"FLUSH";
	m_sourceFile = NULL;
	m_file_duration = 0.0;
	m_video_dec_ctx = NULL;
	m_pvideo_codec = NULL;
	m_screen = NULL;
	m_is_full_screen = 0;
	//初始视频大小
	m_picture_static.GetWindowRect(&m_Picture_rect);
	m_screen_width = m_Picture_rect.right - m_Picture_rect.left;
	m_screen_height = m_Picture_rect.bottom - m_Picture_rect.top;
	//这里是获取到 STATIC控件的大小 
	//这里做判断是因为 如果改变大小时 的 宽 和高 出现 奇数则图像会不正确 或 中断 要偶数
	if (m_screen_width % 2 != 0)
	{
		m_screen_width -= 1;
	}
	if (m_screen_height % 2 != 0)
	{
		m_screen_height -= 1;
	}
	m_Isstop = 0;
	//这里设置文件刚打开时候的状态0 暂停 1播放 
	m_pause_play = 1;
	m_slider_pos = 0;
	m_stream_type = 0;

	//////////////////////////////////////////////////////////////////////////
	//video pram
	m_video_stream_idx = -1;
	m_dbFrameRate = 25.0;
	m_video_duration = 0.040;
	//初始化的时候 设置为static控件的宽,高
	m_dwWidth = m_Picture_rect.right - m_Picture_rect.left;
	m_dwHeight = m_Picture_rect.bottom - m_Picture_rect.top;
	m_video_codecID = AV_CODEC_ID_H264;
	m_video_pixelfromat = AV_PIX_FMT_YUV420P;
	memset(m_spspps, 0, 100);
	m_spspps_size = 0;


	//////////////////////////////////////////////////////////////////////////
	m_Picture_rect.left = 0;
	m_Picture_rect.right = 0;
	m_Picture_rect.top = 0;
	m_Picture_rect.bottom = 0;
	//////////////////////////////////////////////////////////////////////////
	return 1;
}

int CJiaohuDlg::UinitVariable()
{
	VideoPicture* vp;
	int i;
	//释放packet_queue
	if (m_streamstate->read_tid)
	{
		SDL_WaitThread(m_streamstate->read_tid, NULL);
		//SDL_KillThread(m_streamstate->read_tid);
	}
	if (m_streamstate->refresh_tid)
	{
		//SDL_WaitThread(m_streamstate->refresh_tid,NULL);
		SDL_KillThread(m_streamstate->refresh_tid);

	}
	if (m_streamstate->videoq.mutex)
	{
		packet_queue_destroy(&m_streamstate->videoq);
	}

	/* free all pictures */
	for (i = 0; i < VIDEO_PICTURE_QUEUE_SIZE; i++)
	{
		vp = &m_streamstate->pictq[i];
		if (vp->bmp)
		{
			SDL_FreeYUVOverlay(vp->bmp);
			vp->bmp = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	m_streamstate->pFormatCtx = NULL;
	m_streamstate->video_st = NULL;
	m_streamstate->videoq.first_pkt = NULL;
	m_streamstate->videoq.last_pkt = NULL;
	m_streamstate->videoq.nb_packets = 0;
	m_streamstate->videoq.size = 0;
	m_streamstate->videoq.mutex = NULL;
	m_streamstate->videoq.cond = NULL;
	//刷新packet
	if (m_flush_pkt.data)
	{
		av_free_packet(&m_flush_pkt);
	}
	m_streamstate->pictq[0].bmp = NULL;
	m_streamstate->pictq[0].width = 0;
	m_streamstate->pictq[0].height = 0;
	m_streamstate->pictq[0].allocated = 0;
	m_streamstate->pictq[0].pts = 0.0;
	m_streamstate->pictq_size = 0;
	m_streamstate->pictq_rindex = 0;
	m_streamstate->pictq_windex = 0;
	//释放互斥变量 条件变量
	if (m_streamstate->pictq_mutex)
	{
		SDL_DestroyMutex(m_streamstate->pictq_mutex);
	}
	if (m_streamstate->pictq_cond)
	{
		SDL_DestroyCond(m_streamstate->pictq_cond);
	}
	m_streamstate->pictq_mutex = NULL;
	m_streamstate->pictq_cond = NULL;
	m_streamstate->read_tid = NULL;
	m_streamstate->video_tid = NULL;
	m_streamstate->refresh_tid = NULL;
	m_streamstate->video_clock = 0.0;
	m_streamstate->seek_time = 0.0;
	m_streamstate->seek_req = 0;
	m_streamstate->seek_flags = 0;
	m_streamstate->seek_pos = 0;

	//////////////////////////////////////////////////////////////////////////
	//m_fs_screen_width;
	//m_fs_screen_height; 
	//AVPacket m_flush_pkt;
	m_sourceFile = NULL;
	m_file_duration = 0.0;
	m_video_dec_ctx = NULL;
	m_pvideo_codec = NULL;
	//释放窗口 
	if (m_screen)
	{
		SDL_FreeSurface(m_screen);
		m_screen = NULL;
	}
	m_is_full_screen = 0;
	m_screen_width = 0;
	m_screen_height = 0;
	m_Isstop = 1;  //这里必须为1 要不然别的线程可能还没退出 就析构了变量
	m_pause_play = 0;
	m_slider_pos = 0;
	m_stream_type = 0;

	//////////////////////////////////////////////////////////////////////////
	//video pram
	m_video_stream_idx = 0;
	m_dbFrameRate = 0.0;
	m_video_duration = 0.0;
	m_dwWidth = 0;
	m_dwHeight = 0;
	m_video_codecID = AV_CODEC_ID_NONE;
	m_video_pixelfromat = AV_PIX_FMT_NONE;
	memset(m_spspps, 0, 1000);
	m_spspps_size = 0;

	//////////////////////////////////////////////////////////////////////////
	m_Picture_rect.left = 0;
	m_Picture_rect.right = 0;
	m_Picture_rect.top = 0;
	m_Picture_rect.bottom = 0;
	//////////////////////////////////////////////////////////////////////////
	return 1;
}
int CJiaohuDlg::Open_codec_context(int* stream_idx, AVFormatContext* fmt_ctx, enum AVMediaType type)
{
	int ret;
	ret = av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);
	if (ret < 0)
	{
		return ret;
	}
	else
	{
		*stream_idx = ret;
	}
	return 0;
}

int CJiaohuDlg::Access_control_position()
{


	//进度条初始化的位置
	GetDlgItem(IDC_SLIDER_SEEK)->GetWindowRect(&m_rect_SLIDER_PLAYL);//获取控件相对于屏幕的位置
	ScreenToClient(m_rect_SLIDER_PLAYL);//转化为对话框上的相对位置
	m_width_SLIDER_PLAYL = m_rect_SLIDER_PLAYL.right - m_rect_SLIDER_PLAYL.left;  //width为button的宽
	m_height_SLIDER_PLAY = m_rect_SLIDER_PLAYL.bottom - m_rect_SLIDER_PLAYL.top; //height为button的高

	return 1;
}
void CJiaohuDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	
	m_slider_pos = m_slider_seek.GetPos();
	if (pScrollBar->GetSafeHwnd() == m_slider_seek.GetSafeHwnd())        //如果是 文件播放进度条
	{
		//?? seek后的文件结束时间可以优化用video->clock代表文件当前播放的时间 和文件结束时间不匹配
		//首先暂停
		//////////////////////////////////////////////////////////////////////////
		if (m_pause_play) //如果正在播放
		{
			//暂停
			SDL_PauseAudio(1);
			m_pause_play = !m_pause_play;
		}
		//////////////////////////////////////////////////////////////////////////

		if (nSBCode == SB_ENDSCROLL) //结束滚动  
		{
			m_streamstate->seek_time = m_slider_pos - (int)(m_streamstate->video_clock);  //获取改变的时常 可能是整数也可能是负数
			m_streamstate->seek_req = 1;
			m_streamstate->seek_pos = m_slider_pos;
			m_streamstate->seek_flags = AVSEEK_FLAG_FRAME;//m_streamstate->seek_time < 0 ? AVSEEK_FLAG_BACKWARD : 0;
			m_slider_pos = 0;
			//////////////////////////////////////////////////////////////////////////
			//恢复播放
			if (!m_pause_play) //如果正在暂停
			{
				//播放
				SDL_PauseAudio(0);
				m_pause_play = !m_pause_play;
			}
			//////////////////////////////////////////////////////////////////////////
		}
	



	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
void CJiaohuDlg::OnBnClickedButtonPlay()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_pause_play)
	{
		//暂停
		SDL_PauseAudio(1);
		GetDlgItem(IDC_BUTTON_PLAY)->SetWindowText(_T("播放"));
	}
	else
	{
		//播放
		SDL_PauseAudio(0);
		GetDlgItem(IDC_BUTTON_PLAY)->SetWindowText(_T("停止"));
	}
	m_pause_play = !m_pause_play;
}

void CJiaohuDlg::OnBnClickedButtonPlaySlowly()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!slow) {
		normal = 0;
		fast = 0;
		slow = 1;
		GetDlgItem(IDC_BUTTON_PLAY_SLOWLY)->SetWindowText(_T("正常"));
		GetDlgItem(IDC_BUTTON_PLAY_FAST)->SetWindowText(_T("快放"));
	}
	else
	{
		normal = 1;
		fast = 0;
		slow = 0;
		GetDlgItem(IDC_BUTTON_PLAY_SLOWLY)->SetWindowText(_T("慢放"));
		GetDlgItem(IDC_BUTTON_PLAY_FAST)->SetWindowText(_T("快放"));
	}
	if (!m_pause_play)
	{
		//播放
		SDL_PauseAudio(0);
		GetDlgItem(IDC_BUTTON_PLAY)->SetWindowText(_T("停止"));
		m_pause_play = !m_pause_play;
	}
}


void CJiaohuDlg::OnBnClickedButtonPlayFast()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!fast) {
		normal = 0;
		fast = 1;
		slow = 0;
		GetDlgItem(IDC_BUTTON_PLAY_SLOWLY)->SetWindowText(_T("慢放"));
		GetDlgItem(IDC_BUTTON_PLAY_FAST)->SetWindowText(_T("正常"));
	}
	else
	{
		normal = 1;
		fast = 0;
		slow = 0;
		GetDlgItem(IDC_BUTTON_PLAY_SLOWLY)->SetWindowText(_T("慢放"));
		GetDlgItem(IDC_BUTTON_PLAY_FAST)->SetWindowText(_T("快放"));
	}
	if (!m_pause_play)
	{
		//播放
		SDL_PauseAudio(0);
		GetDlgItem(IDC_BUTTON_PLAY)->SetWindowText(_T("停止"));
		m_pause_play = !m_pause_play;
	}
}


void CJiaohuDlg::OnBnClickedButtonPlayFrame()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_pause_play)
	{
		//暂停
		SDL_PauseAudio(1);
		//GetDlgItem(IDC_BUTTON_PLAY)->SetWindowText(_T("播放"));
		m_pause_play = !m_pause_play;
	}
	
	SDL_PauseAudio(0);

	m_pause_play = !m_pause_play;
	Sleep(m_video_duration * 1000);
	SDL_PauseAudio(1);
	m_pause_play = !m_pause_play;
}

/*
void CJiaohuDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	//进度条位置调整
	if (m_IsAccess_control_position == 1)
	{
		GetDlgItem(IDC_SLIDER_SEEK)->MoveWindow(m_rect_SLIDER_PLAYL.left, cy - m_height_SLIDER_PLAY - (m_height_BMAINFRAME - m_rect_SLIDER_PLAYL.bottom),
		cx - 2 * (m_width_MAINFRAME - m_rect_SLIDER_PLAYL.right), m_height_SLIDER_PLAY);
	}

}
*/




void CJiaohuDlg::OnSize(UINT nType, int cx, int cy)
{
	if (nType != SIZE_MINIMIZED)  //判断窗口是不是最小化了，因为窗口最小化之后 ，
			//窗口的长和宽会变成0，当前一次变化的时就会出现除以0的错误操作
	{

		CRect rect;// 获取当前窗口大小
		for (std::list<control*>::iterator it = m_con_list.begin(); it != m_con_list.end(); it++) {
			CWnd* pWnd = GetDlgItem((*it)->Id);//获取ID为woc的空间的句柄
			pWnd->GetWindowRect(&rect);
			ScreenToClient(&rect);//将控件大小转换为在对话框中的区域坐标
			rect.left = (*it)->scale[0] * cx;
			rect.right = (*it)->scale[1] * cx;
			rect.top = (*it)->scale[2] * cy;
			rect.bottom = (*it)->scale[3] * cy;
			pWnd->MoveWindow(rect);//设置控件大小
		}

	}
	GetClientRect(&m_rect);//将变化后的对话框大小设为旧大小
	CDialogEx::OnSize(nType, cx, cy);
	Invalidate(TRUE);
}

static int i = 0;
void CJiaohuDlg::get_control_original_proportion() {
	HWND hwndChild = ::GetWindow(m_hWnd, GW_CHILD);
	while (hwndChild)
	{
		CRect rect;//获取当前窗口的大小
		control* tempcon = new control;
		CWnd* pWnd = GetDlgItem(::GetDlgCtrlID(hwndChild));//获取ID为woc的空间的句柄
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);//将控件大小转换为在对话框中的区域坐标
		tempcon->Id = ::GetDlgCtrlID(hwndChild);//获得控件的ID;
		tempcon->scale[0] = (double)rect.left / m_rect.Width();//注意类型转换，不然保存成long型就直接为0了
		tempcon->scale[1] = (double)rect.right / m_rect.Width();
		tempcon->scale[2] = (double)rect.top / m_rect.Height();
		tempcon->scale[3] = (double)rect.bottom / m_rect.Height();
		m_con_list.push_back(tempcon);
		hwndChild = ::GetWindow(hwndChild, GW_HWNDNEXT);
	}
}
