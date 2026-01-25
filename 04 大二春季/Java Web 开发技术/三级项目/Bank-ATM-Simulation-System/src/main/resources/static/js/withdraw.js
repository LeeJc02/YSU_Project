new Vue({
    el: '#app',
    data: {
        amount: '',
        message: ''
    },
    methods: {
        goBack() {
            window.history.back();
        },
        withdraw() {
            if (this.amount <= 0) {
                this.message = '取款金额必须大于0';
                return;
            }
            axios.post('/account/withdraw', { amount: this.amount })
                .then(response => {
                    this.message = '取款成功！';
                    // 可以在此更新余额或跳转页面
                })
                .catch(error => {
                    this.message = '取款失败，请稍后再试';
                });
        }
    }
});